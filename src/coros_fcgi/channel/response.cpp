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

coros::base::AwaitableFuture coros::fcgi::Response::send_response_body() {
    int content_length = response_body.size();
    int padding_length = 0;
    int remainder = (FCGI_ALIGNMENT_LEN + content_length) % FCGI_ALIGNMENT_LEN;
    if (remainder > 0) {
        padding_length = FCGI_ALIGNMENT_LEN - remainder;
    }
    RecordHeader header { FCGI_VERSION_1, FCGI_STDOUT, request_id, content_length, padding_length};
    co_await header.serialize(socket);
    co_await socket.write(reinterpret_cast<std::byte*>(response_body.data()), response_body.size());
    for (int i = 0; i < padding_length; ++i) {
        co_await socket.write_b(std::byte{0});
    }
}

coros::base::AwaitableFuture coros::fcgi::Response::println(std::string& s) {
    response_body.append(s);
    response_body.append(newline);
    if (response_body.size() <= 1000) {
        co_return;
    }
    co_await send_response_body();
}

coros::base::AwaitableFuture coros::fcgi::Response::print(std::string& s) {
    response_body.append(s);
    if (response_body.size() <= 1000) {
        co_return;
    }
    co_await send_response_body();
}

coros::base::AwaitableFuture coros::fcgi::Response::flush() {
    if (response_body.size() > 0) {
        co_await send_response_body();
    }
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
