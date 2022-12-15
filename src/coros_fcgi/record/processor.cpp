#include "processor.h"
#include "header.h"

#include "coros/async/future.h"
#include "coros/network/socket.h"
#include "coros_fcgi/application/channel.h"
#include "coros_fcgi/commons/integer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::begin_request(RecordHeader& header, 
                                                                         coros::base::Socket& socket) {
    channel_map[header.request_id] = std::make_unique<ApplicationChannel>();
    uint8_t data[8];
    co_await socket.read(reinterpret_cast<std::byte*>(data), header.content_length);
}

coros::base::AwaitableValue<long> get_name_value_pair_length(coros::base::Socket& socket) {
    uint8_t data[4];
    data[0] = static_cast<uint8_t>(co_await socket.read_b());
    if ((data[0] >> 7) == 0) {
        co_return data[0];
    }
    data[0] = data[0] & 0x7f;
    co_await socket.read(reinterpret_cast<std::byte*>(data + 1), 3);
    co_return coros::fcgi::read_uint32_be(data);
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::set_param(RecordHeader& header, 
                                                                     coros::base::Socket& socket) {
    long name_length = co_await get_name_value_pair_length(socket);
    long value_length = co_await get_name_value_pair_length(socket);
    std::string name;
    std::string value;
    name.resize(name_length);
    value.resize(value_length);
    co_await socket.read(reinterpret_cast<std::byte*>(name.data()), name_length);
    co_await socket.read(reinterpret_cast<std::byte*>(value.data()), value_length);
    ApplicationChannel& channel = *channel_map[header.request_id];
    channel.set_variable(std::move(name), std::move(value));
}

coros::base::AwaitableFuture coros::fcgi::RecordProcessor::process(RecordHeader& header, 
                                                                   coros::base::Socket& socket) {
    switch (header.type) {
        case FCGI_BEGIN_REQUEST:
            begin_request(header, socket);
            break;
        case FCGI_PARAMS:
            set_param(header, socket);
            break;
    }
    co_await socket.skip(header.padding_length);
}
