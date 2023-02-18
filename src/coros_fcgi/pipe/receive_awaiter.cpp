#include "receive_awaiter.h"
#include "awaiter.h"

bool coros::fcgi::PipeReceiveAwaiter::await_ready() noexcept {
    return false;
}

std::coroutine_handle<> 
        coros::fcgi::PipeReceiveAwaiter::await_suspend(std::coroutine_handle<> handle) {
    std::lock_guard<std::mutex> guard(pipe_mutex);
    if (available > 0 || is_closed) {
        return handle;
    }
    receiver_opt = handle;
    return get_resume_handle(sender_opt);
}

void coros::fcgi::PipeReceiveAwaiter::await_resume() {
}

