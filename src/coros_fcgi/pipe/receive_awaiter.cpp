#include "receive_awaiter.h"

#include "coros/event/executor.h"

bool coros::fcgi::PipeReceiveAwaiter::await_ready() noexcept {
    std::lock_guard<std::mutex> guard(pipe_mutex);
    return available > 0 || is_closed;
}

bool coros::fcgi::PipeReceiveAwaiter::await_suspend(std::coroutine_handle<> handle) {
    std::lock_guard<std::mutex> guard(pipe_mutex);
    if (available > 0 || is_closed) {
        return false;
    }
    receiver_executor.set_handler([&, handle]() {
        handle.resume();
    });
    sender_executor.on_event();
    return true;
}

void coros::fcgi::PipeReceiveAwaiter::await_resume() {
}

