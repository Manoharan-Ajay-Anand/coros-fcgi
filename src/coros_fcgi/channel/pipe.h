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
        std::mutex& sender_mutex;
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
        base::Socket*& socket;
        std::mutex& sender_mutex;
        base::EventHandlerExecutor& receiver_executor;
        base::EventHandlerExecutor& sender_executor;
        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> handle);
        void await_resume();
    };

    class Pipe {
        private:
            int available;
            base::Socket* socket;
            std::mutex sender_mutex;
            base::EventHandlerExecutor receiver_executor;
            base::EventHandlerExecutor sender_executor;
        public:
            Pipe(base::ThreadPool& thread_pool);
            base::AwaitableFuture read(std::byte* dest, int size);
            base::AwaitableValue<std::byte> read_b();
            PipeSendAwaiter send(base::Socket* content_socket, int content_length);
    };
}

#endif
