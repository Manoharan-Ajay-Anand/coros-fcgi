#include "send_awaiter.h"
#include "awaiter.h"
#include "coros/async/thread_pool.h"

#include <coroutine>

bool coros::fcgi::PipeSendAwaiter::await_ready() noexcept {
    if (content_length > 0) {
        return false;
    }
    std::coroutine_handle<> receiver_handle;
    {
        std::lock_guard<std::mutex> guard(pipe_mutex);
        is_closed = true;
        receiver_handle = get_resume_handle(receiver_opt);
    }
    thread_pool.run(receiver_handle);
    return true;
}

std::coroutine_handle<> 
        coros::fcgi::PipeSendAwaiter::await_suspend(std::coroutine_handle<> handle) {
    std::lock_guard<std::mutex> guard(pipe_mutex);
    available += content_length;
    sender_opt = handle;
    return get_resume_handle(receiver_opt);
}

void coros::fcgi::PipeSendAwaiter::await_resume() {
}
