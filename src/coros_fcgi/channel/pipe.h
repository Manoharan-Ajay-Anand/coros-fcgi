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
        int& available;
        bool& is_closed;
        std::mutex& pipe_mutex;
        base::EventHandlerExecutor& receiver_executor;
        base::EventHandlerExecutor& sender_executor;
        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> handle);
        void await_resume();
    };

    struct PipeSendAwaiter {
        int content_length;
        base::Socket* content_socket;
        int& available;
        bool& is_closed;
        base::Socket*& socket;
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
            int available;
            base::Socket* socket;
            std::mutex pipe_mutex;
            base::EventHandlerExecutor receiver_executor;
            base::EventHandlerExecutor sender_executor;
        public:
            Pipe(base::ThreadPool& thread_pool);
            base::AwaitableValue<bool> has_ended();
            base::AwaitableFuture read(std::byte* dest, int size);
            base::AwaitableValue<std::byte> read_b();
            PipeSendAwaiter send(base::Socket* content_socket, int content_length);
    };
}

#endif
