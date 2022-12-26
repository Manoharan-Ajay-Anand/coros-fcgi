#include "pipe.h"

#include "coros/async/future.h"
#include "coros/network/socket.h"

#include <algorithm>
#include <mutex>
#include <stdexcept>

coros::fcgi::Pipe::Pipe(base::ThreadPool& thread_pool): receiver_executor(thread_pool), 
                                                        sender_executor(thread_pool) {
}

coros::base::AwaitableFuture coros::fcgi::Pipe::read(std::byte* dest, int size) {
    while (size > 0) {
        co_await PipeReceiveAwaiter { available, sender_mutex, receiver_executor, sender_executor };
        {
            std::lock_guard<std::mutex> guard(sender_mutex);
            int size_to_read = std::min(size, available);
            co_await socket->read(dest, size_to_read);
            dest += size_to_read;
            size -= size_to_read;
            available -= size_to_read;
        }
    }
}

coros::base::AwaitableValue<std::byte> coros::fcgi::Pipe::read_b() {
    co_await PipeReceiveAwaiter { available, sender_mutex, receiver_executor, sender_executor };
    std::byte b;
    {
        std::lock_guard<std::mutex> guard(sender_mutex);
        b = co_await socket->read_b();
        available -= 1;
    }
    co_return b;
}

coros::fcgi::PipeSendAwaiter coros::fcgi::Pipe::send(base::Socket* content_socket, 
                                                     int content_length) {
    return { content_length, content_socket, available, socket, sender_mutex, 
             receiver_executor, sender_executor };
}

bool coros::fcgi::PipeReceiveAwaiter::await_ready() noexcept {
    std::lock_guard<std::mutex> guard(sender_mutex);
    return available > 0;
}

void coros::fcgi::PipeReceiveAwaiter::await_suspend(std::coroutine_handle<> handle) {
    receiver_executor.set_handler([&, handle]() {
        handle.resume();
    });
    sender_executor.on_event();
}

void coros::fcgi::PipeReceiveAwaiter::await_resume() {
    if (available == 0) {
        throw std::runtime_error("Pipe not available for receive");
    }
}

bool coros::fcgi::PipeSendAwaiter::await_ready() noexcept {
    return false;
}

void coros::fcgi::PipeSendAwaiter::await_suspend(std::coroutine_handle<> handle) {
    sender_executor.set_handler([&, handle]() {
        handle.resume();
    });
    {
        std::lock_guard<std::mutex> guard(sender_mutex);
        available += content_length;
        socket = content_socket;
    }
    receiver_executor.on_event();
}

void coros::fcgi::PipeSendAwaiter::await_resume() {
}
