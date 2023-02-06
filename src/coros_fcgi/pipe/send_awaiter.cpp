#include "send_awaiter.h"

#include "coros/event/executor.h"

bool coros::fcgi::PipeSendAwaiter::await_ready() noexcept {
    if (content_length > 0) {
        return false;
    }
    std::lock_guard<std::mutex> guard(pipe_mutex);
    is_closed = true;
    receiver_executor.on_event();
    return true;
}

void coros::fcgi::PipeSendAwaiter::await_suspend(std::coroutine_handle<> handle) {
    std::lock_guard<std::mutex> guard(pipe_mutex);
    available += content_length;
    sender_executor.set_handler([&, handle]() {
        handle.resume();
    });
    receiver_executor.on_event();
}

void coros::fcgi::PipeSendAwaiter::await_resume() {
}
