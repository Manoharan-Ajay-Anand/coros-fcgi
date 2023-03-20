#ifndef COROS_FCGI_HANDLER_H
#define COROS_FCGI_HANDLER_H

#include "coros/async/future.h"

#include <string>
#include <unordered_map>

namespace coros::fcgi {
    struct Channel;

    struct Request;

    class Response;

    class FcgiHandler {
        public:
            base::Future on_request(Channel& channel);
            virtual base::AwaitableFuture process_request(Request& request,
                                                          Response& response) = 0;
    };
}

#endif
