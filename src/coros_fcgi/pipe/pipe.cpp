#include "pipe.h"

#include "coros/async/future.h"
#include "coros/network/socket.h"

#include <algorithm>
#include <mutex>
#include <stdexcept>


coros::fcgi::Pipe::Pipe(base::ThreadPool& thread_pool, 
                        base::Socket& socket): thread_pool(thread_pool), socket(socket) {
    available = 0;
    is_closed = false;
}

coros::base::AwaitableValue<bool> coros::fcgi::Pipe::has_ended() {
    co_await PipeReceiveAwaiter {
        available, is_closed, pipe_mutex, receiver_opt, sender_opt
    };
    co_return available == 0;
}

coros::base::AwaitableFuture coros::fcgi::Pipe::read(std::byte* dest, long long size) {
    while (size > 0) {
        {
            std::lock_guard<std::mutex> guard(pipe_mutex);
            if (available > 0 || is_closed) {
                if (available == 0) {
                    throw std::runtime_error("Pipe read: cannot read as pipe has ended");
                }
                long long size_to_read = std::min(size, available);
                co_await socket.read(dest, size_to_read, true);
                dest += size_to_read;
                size -= size_to_read;
                available -= size_to_read;
                continue;
            }
        }
        co_await PipeReceiveAwaiter {
            available, is_closed, pipe_mutex, receiver_opt, sender_opt
        };
    }
}

coros::fcgi::PipeSendAwaiter coros::fcgi::Pipe::send(long long content_length) {
    return {
        content_length, 
        available, is_closed, pipe_mutex, 
        receiver_opt, sender_opt,
        thread_pool
    };
}
