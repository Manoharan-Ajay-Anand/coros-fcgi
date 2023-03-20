#include "handler.h"

#include "coros/async/future.h"

#include "coros_fcgi/channel/channel.h"
#include "coros_fcgi/commons/integer.h"
#include "coros_fcgi/application/request.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>

coros::base::AwaitableValue<long long> get_param_detail_length(coros::fcgi::Pipe& pipe) {
    uint8_t data[4];
    co_await pipe.read(reinterpret_cast<std::byte*>(data), 1);
    if ((data[0] >> 7) == 0) {
        co_return data[0];
    }
    data[0] = data[0] & 0x7f;
    co_await pipe.read(reinterpret_cast<std::byte*>(data + 1), 3);
    co_return coros::fcgi::read_uint32_be(data);
}

coros::base::Future coros::fcgi::FcgiHandler::on_request(Channel& channel) {
    try {
        Pipe& variables = channel.fcgi_variables;
        std::unordered_map<std::string, std::string> variable_values;
        while (!(co_await variables.has_ended())) {
            long long name_length = co_await get_param_detail_length(variables);
            long long value_length = co_await get_param_detail_length(variables);
            std::string name, value;
            name.resize(name_length);
            value.resize(value_length);
            co_await variables.read(reinterpret_cast<std::byte*>(name.data()), name.size());
            co_await variables.read(reinterpret_cast<std::byte*>(value.data()), value.size());
            variable_values[std::move(name)] = std::move(value);
        }
        Request request { variable_values, channel.fcgi_stdin };
        co_await this->process_request(request, channel.response);
        co_await channel.response.close();
    } catch (std::runtime_error error) {
        std::cerr << error.what() << std::endl;
    }
}
