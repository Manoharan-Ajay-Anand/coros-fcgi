#include "header.h"
#include "coros/network/socket.h"
#include "coros_fcgi/commons/integer.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

coros::base::AwaitableFuture coros::fcgi::RecordHeader::parse(coros::base::Socket& socket) {
    uint8_t header_data[FCGI_HEADER_LEN];
    co_await socket.read(reinterpret_cast<std::byte*>(header_data), FCGI_HEADER_LEN);
    this->version = header_data[0];
    this->type = header_data[1];
    this->request_id = coros::fcgi::read_uint16_be(header_data + 2);
    this->content_length = coros::fcgi::read_uint16_be(header_data + 4);
    this->padding_length = header_data[6];
}
