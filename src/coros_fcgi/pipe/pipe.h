#ifndef COROS_FCGI_PIPE_PIPE_H
#define COROS_FCGI_PIPE_PIPE_H

#include "receive_awaiter.h"
#include "send_awaiter.h"

#include "coros/async/future.h"

#include <mutex>
#include <optional>
#include <coroutine>

namespace coros::base {
    class Socket;

    class ThreadPool;
}

namespace coros::fcgi {
    class Pipe {
        private:
            bool is_closed;
            long long available;
            base::ThreadPool& thread_pool;
            base::Socket& socket;
            std::mutex pipe_mutex;
            std::optional<std::coroutine_handle<>> receiver_opt;
            std::optional<std::coroutine_handle<>> sender_opt;
        public:
            Pipe(base::ThreadPool& thread_pool, base::Socket& socket);
            base::AwaitableValue<bool> has_ended();
            base::AwaitableFuture read(std::byte* dest, long long size);
            PipeSendAwaiter send(long long content_length);
    };
}

#endif
