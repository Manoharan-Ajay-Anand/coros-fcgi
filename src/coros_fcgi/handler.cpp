#include "handler.h"

#include "coros/async/future.h"

#include "coros_fcgi/channel/channel.h"
#include "coros_fcgi/channel/pipe.h"
#include "coros_fcgi/commons/integer.h"

#include <cstddef>
#include <iostream>
#include <string>

coros::base::AwaitableValue<long long> get_param_detail_length(coros::fcgi::Pipe& pipe) {
    uint8_t data[4];
    data[0] = static_cast<uint8_t>(co_await pipe.read_b());
    if ((data[0] >> 7) == 0) {
        co_return data[0];
    }
    data[0] = data[0] & 0x7f;
    co_await pipe.read(reinterpret_cast<std::byte*>(data + 1), 3);
    co_return coros::fcgi::read_uint32_be(data);
}

coros::base::Future coros::fcgi::FcgiHandler::on_request(Channel& channel) {
    Pipe& variables = channel.fcgi_variables;
    while (!(co_await variables.has_ended())) {
        long long name_length = co_await get_param_detail_length(variables);
        long long value_length = co_await get_param_detail_length(variables);
        std::string name, value;
        name.resize(name_length);
        value.resize(value_length);
        co_await variables.read(reinterpret_cast<std::byte*>(name.data()), name.size());
        co_await variables.read(reinterpret_cast<std::byte*>(value.data()), value.size());
        std::cout << name << ": " << value << std::endl;
    }
    co_await this->process_request(channel);
}
