#ifndef COROS_FCGI_PIPE_SEND_AWAITER_H
#define COROS_FCGI_PIPE_SEND_AWAITER_H

#include <atomic>
#include <coroutine>
#include <mutex>
#include <optional>

namespace coros::base {
    class ThreadPool;
}

namespace coros::fcgi {
    struct PipeSendAwaiter {
        long long content_length;
        long long& available;
        bool& is_closed;
        std::mutex& pipe_mutex;
        std::optional<std::coroutine_handle<>>& receiver_opt;
        std::optional<std::coroutine_handle<>>& sender_opt;
        base::ThreadPool& thread_pool;
        bool await_ready() noexcept;
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);
        void await_resume();
    };
}

#endif
