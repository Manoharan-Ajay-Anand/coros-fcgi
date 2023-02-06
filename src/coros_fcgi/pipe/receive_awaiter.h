#ifndef COROS_FCGI_PIPE_RECEIVE_AWAITER_H
#define COROS_FCGI_PIPE_RECEIVE_AWAITER_H

#include <atomic>
#include <coroutine>
#include <mutex>

namespace coros::base {
    class EventHandlerExecutor;
}

namespace coros::fcgi {
    struct PipeReceiveAwaiter {
        std::atomic_llong& available;
        std::atomic_bool& is_closed;
        std::mutex& pipe_mutex;
        base::EventHandlerExecutor& receiver_executor;
        base::EventHandlerExecutor& sender_executor;
        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> handle);
        void await_resume();
    };
}

#endif