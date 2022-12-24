#ifndef COROS_FCGI_APP_H
#define COROS_FCGI_APP_H

#include "coros/app.h"
#include "coros/async/future.h"
#include "record/processor.h"

namespace coros::base {
    class ThreadPool;
}

namespace coros::fcgi {
    struct Channel;

    class FcgiHandler {
        public:
            virtual base::Future handle_request(Channel& channel) = 0;
    };

    class FcgiApplication : public base::ServerApplication {
        private:
            RecordProcessor processor;
        public:
            FcgiApplication(base::ThreadPool& thread_pool, FcgiHandler& fcgi_handler);
            base::Future on_request(base::Server& server, std::shared_ptr<base::Socket> socket);
    };
}

#endif
