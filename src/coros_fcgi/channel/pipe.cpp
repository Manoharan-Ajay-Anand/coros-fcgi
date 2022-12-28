#include "pipe.h"

#include "coros/async/future.h"
#include "coros/network/socket.h"

#include <algorithm>
#include <mutex>
#include <stdexcept>

coros::fcgi::Pipe::Pipe(base::ThreadPool& thread_pool): receiver_executor(thread_pool), 
                                                        sender_executor(thread_pool),
                                                        is_closed(false) {
}

coros::base::AwaitableValue<bool> coros::fcgi::Pipe::has_ended() {
    co_await PipeReceiveAwaiter {
        available, is_closed, pipe_mutex, receiver_executor, sender_executor
    };
    std::lock_guard<std::mutex> guard(pipe_mutex);
    co_return available == 0;
}

coros::base::AwaitableFuture coros::fcgi::Pipe::read(std::byte* dest, int size) {
    while (size > 0) {
        bool pipe_ended = co_await has_ended();
        if (pipe_ended) {
            throw std::runtime_error("Pipe read: cannot read as pipe has ended");
        }
        std::lock_guard<std::mutex> guard(pipe_mutex);
        int size_to_read = std::min(size, available);
        co_await socket->read(dest, size_to_read);
        dest += size_to_read;
        size -= size_to_read;
        available -= size_to_read;
    }
}

coros::base::AwaitableValue<std::byte> coros::fcgi::Pipe::read_b() {
    bool pipe_ended = co_await has_ended();
    if (pipe_ended) {
        throw std::runtime_error("Pipe read_b: cannot read as closed");
    }
    std::lock_guard<std::mutex> guard(pipe_mutex);
    std::byte b = co_await socket->read_b();
    available -= 1;
    co_return b;
}

coros::fcgi::PipeSendAwaiter coros::fcgi::Pipe::send(base::Socket* content_socket, 
                                                     int content_length) {
    return {
        content_length, content_socket, 
        available, is_closed, socket, pipe_mutex, 
        receiver_executor, sender_executor
    };
}

bool coros::fcgi::PipeReceiveAwaiter::await_ready() noexcept {
    std::lock_guard<std::mutex> guard(pipe_mutex);
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
    if (content_length == 0) {
        {
            std::lock_guard<std::mutex> guard(pipe_mutex);
            is_closed = true;
            receiver_executor.on_event();
        }
        return true;
    }
    return false;
}

void coros::fcgi::PipeSendAwaiter::await_suspend(std::coroutine_handle<> handle) {
    std::lock_guard<std::mutex> guard(pipe_mutex);
    sender_executor.set_handler([&, handle]() {
        handle.resume();
    });
    available += content_length;
    socket = content_socket;
    receiver_executor.on_event();
}

void coros::fcgi::PipeSendAwaiter::await_resume() {
}
