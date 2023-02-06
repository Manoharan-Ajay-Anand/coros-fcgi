#ifndef COROS_FCGI_PIPE_PIPE_H
#define COROS_FCGI_PIPE_PIPE_H

#include "receive_awaiter.h"
#include "send_awaiter.h"

#include "coros/async/future.h"
#include "coros/event/executor.h"

#include <mutex>

namespace coros::base {
    class Socket;

    class ThreadPool;
}

namespace coros::fcgi {
    class Pipe {
        private:
            bool is_closed;
            long long available;
            base::Socket& socket;
            std::mutex pipe_mutex;
            base::EventHandlerExecutor receiver_executor;
            base::EventHandlerExecutor sender_executor;
        public:
            Pipe(base::ThreadPool& thread_pool, base::Socket& socket);
            base::AwaitableValue<bool> has_ended();
            base::AwaitableFuture read(std::byte* dest, long long size);
            PipeSendAwaiter send(long long content_length);
    };
}

#endif
