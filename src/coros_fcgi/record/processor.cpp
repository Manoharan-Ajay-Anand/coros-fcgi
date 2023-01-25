#include "processor.h"
#include "header.h"

#include "coros/async/future.h"
#include "coros/async/thread_pool.h"
#include "coros/network/socket.h"

#include "coros_fcgi/app.h"
#include "coros_fcgi/handler.h"
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
                                                                         base::Socket& socket) {
    uint8_t data[FCGI_BEGIN_REQUEST_LEN];
    co_await socket.read(reinterpret_cast<std::byte*>(data), FCGI_BEGIN_REQUEST_LEN, true);
    int request_id = header.request_id;
    bool keep_conn = (data[2] & FCGI_KEEP_CONN) == FCGI_KEEP_CONN;
    channel_map[request_id] = std::make_unique<Channel>(request_id, socket, keep_conn, thread_pool);
    thread_pool.run([&, request_id]() {
        fcgi_handler.on_request(*channel_map[request_id]);
    });
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::set_param(RecordHeader& header, 
                                                                     base::Socket& socket) {
    Channel& channel = *channel_map[header.request_id];
    co_await channel.fcgi_variables.send(header.content_length);
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::receive_stdin(RecordHeader& header, 
                                                                         base::Socket& socket) {
    Channel& channel = *channel_map[header.request_id];
    co_await channel.fcgi_stdin.send(header.content_length);
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::receive_data(RecordHeader& header, 
                                                                        base::Socket& socket) {
    Channel& channel = *channel_map[header.request_id];
    co_await channel.fcgi_data.send(header.content_length);
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::handle_unknown_type(RecordHeader& header, 
                                                                               base::Socket& socket) {
    co_await socket.skip(header.content_length, true);
    RecordHeader res_header(FCGI_VERSION_1, FCGI_UNKNOWN_TYPE, 0, 8);
    char data[8];
    data[0] = header.type;
    co_await res_header.serialize(socket);
    co_await socket.write(reinterpret_cast<std::byte*>(data), 8);
    co_await res_header.pad(socket);
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
            co_await handle_unknown_type(header, socket);
    }
    co_await socket.skip(header.padding_length, true);
}
