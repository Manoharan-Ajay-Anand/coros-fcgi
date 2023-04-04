#ifndef COROS_FCGI_APPLICATION_ENDPOINT_H
#define COROS_FCGI_APPLICATION_ENDPOINT_H

#include "coros/async/future.h"

namespace coros::fcgi {
    struct Request;

    class Response;

    class FcgiEndpoint {
        public:
            virtual base::AwaitableFuture on_request(Request& request,
                                                     Response& response) = 0;
    };
}

#endif
