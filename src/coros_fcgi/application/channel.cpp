#include "channel.h"

void coros::fcgi::ApplicationChannel::set_variable(std::string& name, std::string& value) {
    variables[name] = value;
}

void coros::fcgi::ApplicationChannel::set_variable(std::string&& name, std::string&& value) {
    variables[std::move(name)] = std::move(value);
}
