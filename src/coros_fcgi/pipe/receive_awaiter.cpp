#include "receive_awaiter.h"

#include "coros/event/executor.h"

bool coros::fcgi::PipeReceiveAwaiter::await_ready() noexcept {
    return available > 0 || is_closed;
}

void coros::fcgi::PipeReceiveAwaiter::await_suspend(std::coroutine_handle<> handle) {
    std::lock_guard<std::mutex> guard(pipe_mutex);
    receiver_executor.set_handler([&, handle]() {
        handle.resume();
    });
    sender_executor.on_event();
}

void coros::fcgi::PipeReceiveAwaiter::await_resume() {
}

