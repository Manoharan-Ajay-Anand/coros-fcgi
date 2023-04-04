#ifndef COROS_FCGI_HANDLER_H
#define COROS_FCGI_HANDLER_H

#include "coros/async/future.h"

#include <string>
#include <unordered_map>

namespace coros::fcgi {
    struct Channel;

    struct Request;

    class Response;

    class FcgiEndpoint;

    class FcgiHandler {
        private:
            std::unordered_map<std::string, FcgiEndpoint*> route_endpoint_map;
            base::AwaitableFuture route(Request& request, Response& response);
        public:
            void add_endpoint(const std::string& route, FcgiEndpoint& endpoint);
            base::Future on_request(Channel& channel);
    };
}

#endif
