#include "processor.h"
#include "header.h"

#include "coros/async/future.h"
#include "coros/async/thread_pool.h"
#include "coros/network/socket.h"
#include "coros_fcgi/app.h"
#include "coros_fcgi/channel/channel.h"
#include "coros_fcgi/commons/integer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <sstream>

coros::fcgi::RecordProcessor::RecordProcessor(base::ThreadPool& thread_pool, 
                                              FcgiHandler& fcgi_handler): thread_pool(thread_pool),
                                                                          fcgi_handler(fcgi_handler) {
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::begin_request(RecordHeader& header, 
                                                                         coros::base::Socket& socket) {
    uint8_t data[FCGI_BEGIN_REQUEST_LEN];
    co_await socket.read(reinterpret_cast<std::byte*>(data), FCGI_BEGIN_REQUEST_LEN);
    bool keep_conn = (data[2] & FCGI_KEEP_CONN) == FCGI_KEEP_CONN;
    channel_map[header.request_id] = 
            std::make_unique<Channel>(header.request_id, socket, keep_conn, thread_pool);
}

struct ParamDetail {
    int num_bytes;
    long long content_size;

    inline long long size() {
        return num_bytes + content_size;
    }
};

coros::base::AwaitableValue<ParamDetail> get_param_detail_length(coros::base::Socket& socket) {
    uint8_t data[4];
    data[0] = static_cast<uint8_t>(co_await socket.read_b());
    if ((data[0] >> 7) == 0) {
        co_return { 1, data[0] };
    }
    data[0] = data[0] & 0x7f;
    co_await socket.read(reinterpret_cast<std::byte*>(data + 1), 3);
    co_return { 4, coros::fcgi::read_uint32_be(data) };
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::set_param(RecordHeader& header, 
                                                                     coros::base::Socket& socket) {
    Channel& channel = *channel_map[header.request_id];
    if (header.content_length == 0) {
        thread_pool.run([&]() {
            fcgi_handler.handle_request(channel);
        });
        co_return;
    }
    int content_length = header.content_length;
    while (content_length > 0) {
        ParamDetail name_detail = co_await get_param_detail_length(socket);
        ParamDetail value_detail = co_await get_param_detail_length(socket);
        std::string name;
        std::string value;
        name.resize(name_detail.content_size);
        value.resize(value_detail.content_size);
        co_await socket.read(reinterpret_cast<std::byte*>(name.data()), name_detail.content_size);
        co_await socket.read(reinterpret_cast<std::byte*>(value.data()), value_detail.content_size);
        content_length -= name_detail.size() + value_detail.size();
        channel.variables[std::move(name)] = std::move(value);
    }
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::receive_stdin(RecordHeader& header, 
                                                                         coros::base::Socket& socket) {
    Channel& channel = *channel_map[header.request_id];
    co_await channel.fcgi_stdin.send(&socket, header.content_length);
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::receive_data(RecordHeader& header, 
                                                                        coros::base::Socket& socket) {
    Channel& channel = *channel_map[header.request_id];
    co_await channel.fcgi_data.send(&socket, header.content_length);
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::process(RecordHeader& header, 
                                                                   coros::base::Socket& socket) {
    switch (header.type) {
        case FCGI_BEGIN_REQUEST:
            co_await begin_request(header, socket);
            break;
        case FCGI_PARAMS:
            co_await set_param(header, socket);
            break;
        case FCGI_STDIN:
            co_await receive_stdin(header, socket);
            break;
        case FCGI_DATA:
            co_await receive_data(header, socket);
            break;
        default:
            std::stringstream ss;
            ss << "Unknown header type: " << header.type;
            throw std::runtime_error(ss.str());
    }
    co_await socket.skip(header.padding_length);
}
