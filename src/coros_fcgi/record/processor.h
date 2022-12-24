#ifndef COROS_FCGI_RECORD_PROCESSOR_H
#define COROS_FCGI_RECORD_PROCESSOR_H

#include "coros/async/future.h"

#include <memory>
#include <unordered_map>

namespace coros::base {
    class Socket;

    class ThreadPool;
}

namespace coros::fcgi {
    struct RecordHeader;

    struct Channel;

    class FcgiHandler;

    class RecordProcessor {
        private:
            std::unordered_map<int, std::unique_ptr<Channel>> channel_map;
            base::ThreadPool& thread_pool;
            FcgiHandler& fcgi_handler;
            base::AwaitableFuture begin_request(RecordHeader& header, base::Socket& socket);
            base::AwaitableFuture set_param(RecordHeader& header, base::Socket& socket);
            base::AwaitableFuture receive_stdin(RecordHeader& header, base::Socket& socket);
            base::AwaitableFuture receive_data(RecordHeader& header, base::Socket& socket);
        public:
            RecordProcessor(base::ThreadPool& thread_pool, FcgiHandler& fcgi_handler);
            base::AwaitableFuture process(RecordHeader& header, base::Socket& socket);
    };
}

#endif
