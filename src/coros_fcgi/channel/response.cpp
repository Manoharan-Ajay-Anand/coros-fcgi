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
    RecordHeader header(FCGI_VERSION_1, FCGI_STDOUT, request_id, size);
    co_await header.serialize(socket);
    co_await socket.write(data, size);
    co_await header.pad(socket);
}

coros::base::AwaitableFuture coros::fcgi::Response::println(std::string s) {
    s.append(newline);
    co_await write(reinterpret_cast<std::byte*>(s.data()), s.size());
}

coros::base::AwaitableFuture coros::fcgi::Response::print(std::string s) {
    co_await write(reinterpret_cast<std::byte*>(s.data()), s.size());
}

coros::base::AwaitableFuture coros::fcgi::Response::flush() {
    RecordHeader stdout_header(FCGI_VERSION_1, FCGI_STDOUT, request_id, 0);
    co_await stdout_header.serialize(socket);
    uint8_t end_request_data[FCGI_END_REQUEST_LEN];
    end_request_data[4] = FCGI_REQUEST_COMPLETE;
    RecordHeader end_request_header(FCGI_VERSION_1, FCGI_END_REQUEST, request_id, FCGI_END_REQUEST_LEN);
    co_await end_request_header.serialize(socket);
    co_await socket.write(reinterpret_cast<std::byte*>(end_request_data), FCGI_END_REQUEST_LEN);
    co_await socket.flush();
    if (!keep_conn) {
        socket.close_socket();
    }
}
