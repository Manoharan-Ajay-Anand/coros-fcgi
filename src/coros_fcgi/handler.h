#ifndef COROS_FCGI_HANDLER_H
#define COROS_FCGI_HANDLER_H

#include "coros/async/future.h"

namespace coros::fcgi {
    struct Channel;

    class FcgiHandler {
        public:
            base::Future on_request(Channel& channel);
            virtual base::AwaitableFuture process_request(Channel& channel) = 0;
    };
}

#endif
