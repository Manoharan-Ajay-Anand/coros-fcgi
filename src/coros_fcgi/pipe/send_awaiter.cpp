#include "send_awaiter.h"

#include "coros/event/executor.h"

bool coros::fcgi::PipeSendAwaiter::await_ready() noexcept {
    if (content_length > 0) {
        return false;
    }
    if (!is_closed.exchange(true)) {
        std::lock_guard<std::mutex> guard(pipe_mutex);
        receiver_executor.on_event();
    }
    return true;
}

void coros::fcgi::PipeSendAwaiter::await_suspend(std::coroutine_handle<> handle) {
    available += content_length;
    std::lock_guard<std::mutex> guard(pipe_mutex);
    sender_executor.set_handler([&, handle]() {
        handle.resume();
    });
    receiver_executor.on_event();
}

void coros::fcgi::PipeSendAwaiter::await_resume() {
}
