#ifndef COROS_FCGI_PIPE_RECEIVE_AWAITER_H
#define COROS_FCGI_PIPE_RECEIVE_AWAITER_H

#include <atomic>
#include <coroutine>
#include <mutex>
#include <optional>

namespace coros::fcgi {
    struct PipeReceiveAwaiter {
        long long& available;
        bool& is_closed;
        std::mutex& pipe_mutex;
        std::optional<std::coroutine_handle<>>& receiver_opt;
        std::optional<std::coroutine_handle<>>& sender_opt;
        bool await_ready() noexcept;
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);
        void await_resume();
    };
}

#endif
