#ifndef COROS_FCGI_APPLICATION_REQUEST_H
#define COROS_FCGI_APPLICATION_REQUEST_H

#include <string>
#include <unordered_map>

namespace coros::fcgi {
    class Pipe;

    struct Request {
        std::unordered_map<std::string, std::string>& variables;
        Pipe& body;
    };
}

#endif
