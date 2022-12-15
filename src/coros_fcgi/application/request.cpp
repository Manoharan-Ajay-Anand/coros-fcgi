#include "request.h"

#include <string>
#include <unordered_map>

void coros::fcgi::ApplicationRequest::set_param(std::string& name, std::string& value) {
    params[name] = value;
}

bool coros::fcgi::ApplicationRequest::has_param(std::string& name) {
    return params.contains(name);
}

std::string& coros::fcgi::ApplicationRequest::get_param(std::string& name) {
    return params[name];
}

