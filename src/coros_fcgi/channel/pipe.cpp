#include "pipe.h"

#include "coros/async/future.h"
#include "coros/network/socket.h"

#include <algorithm>
#include <mutex>
#include <stdexcept>

coros::fcgi::Pipe::Pipe(base::ThreadPool& thread_pool, base::Socket& socket)
        : receiver_executor(thread_pool), sender_executor(thread_pool),
          socket(socket), is_closed(false) {
}

coros::base::AwaitableValue<bool> coros::fcgi::Pipe::has_ended() {
    co_await PipeReceiveAwaiter {
        available, is_closed, pipe_mutex, receiver_executor, sender_executor
    };
    co_return available == 0;
}

coros::base::AwaitableFuture coros::fcgi::Pipe::read(std::byte* dest, long long size) {
    while (size > 0) {
        bool pipe_ended = co_await has_ended();
        if (pipe_ended) {
            throw std::runtime_error("Pipe read: cannot read as pipe has ended");
        }
        long long size_to_read = std::min(size, available.load());
        co_await socket.read(dest, size_to_read, true);
        dest += size_to_read;
        size -= size_to_read;
        available -= size_to_read;
    }
}

coros::fcgi::PipeSendAwaiter coros::fcgi::Pipe::send(long long content_length) {
    return {
        content_length, 
        available, is_closed, pipe_mutex, 
        receiver_executor, sender_executor
    };
}

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
