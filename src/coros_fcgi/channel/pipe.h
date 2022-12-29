#ifndef COROS_FCGI_CHANNEL_PIPE_H
#define COROS_FCGI_CHANNEL_PIPE_H

#include "coros/async/future.h"
#include "coros/event/executor.h"

#include <mutex>

namespace coros::base {
    class Socket;

    class ThreadPool;
}

namespace coros::fcgi {
    struct PipeReceiveAwaiter {
        long long& available;
        bool& is_closed;
        std::mutex& pipe_mutex;
        base::EventHandlerExecutor& receiver_executor;
        base::EventHandlerExecutor& sender_executor;
        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> handle);
        void await_resume();
    };

    struct PipeSendAwaiter {
        long long content_length;
        long long& available;
        bool& is_closed;
        std::mutex& pipe_mutex;
        base::EventHandlerExecutor& receiver_executor;
        base::EventHandlerExecutor& sender_executor;
        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> handle);
        void await_resume();
    };

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
            base::AwaitableValue<std::byte> read_b();
            PipeSendAwaiter send(long long content_length);
    };
}

#endif
