#include "response.h"

#include "coros/async/future.h"
#include "coros/network/socket.h"

#include "coros_fcgi/commons/integer.h"
#include "coros_fcgi/record/header.h"

#include <cstddef>
#include <string>

#define NEWLINE_SIZE 2

std::string newline("\r\n");

coros::fcgi::Response::Response(int request_id, base::Socket& socket, bool keep_conn) 
        : request_id(request_id), socket(socket), keep_conn(keep_conn) {
}

coros::base::AwaitableFuture coros::fcgi::Response::write(std::byte* data, int size) {
    int padding_length = 0;
    int remainder = (FCGI_HEADER_LEN + size) % FCGI_ALIGNMENT_LEN;
    if (remainder > 0) {
        padding_length = FCGI_ALIGNMENT_LEN - remainder;
    }
    RecordHeader header { FCGI_VERSION_1, FCGI_STDOUT, request_id, size, padding_length};
    co_await header.serialize(socket);
    co_await socket.write(data, size);
    for (int i = 0; i < padding_length; ++i) {
        co_await socket.write_b(std::byte{0});
    }
}

coros::base::AwaitableFuture coros::fcgi::Response::println(std::string s) {
    s.append(newline);
    co_await write(reinterpret_cast<std::byte*>(s.data()), s.size());
}

coros::base::AwaitableFuture coros::fcgi::Response::print(std::string s) {
    co_await write(reinterpret_cast<std::byte*>(s.data()), s.size());
}

coros::base::AwaitableFuture coros::fcgi::Response::flush() {
    RecordHeader stdout_header { FCGI_VERSION_1, FCGI_STDOUT, request_id, 0, 0};
    co_await stdout_header.serialize(socket);
    uint8_t data[FCGI_END_REQUEST_LEN];
    data[4] = FCGI_REQUEST_COMPLETE;
    RecordHeader end_header { FCGI_VERSION_1, FCGI_END_REQUEST, request_id, 8, 0};
    co_await end_header.serialize(socket);
    co_await socket.write(reinterpret_cast<std::byte*>(data), 8);
    co_await socket.flush();
    if (!keep_conn) {
        socket.close_socket();
    }
}
