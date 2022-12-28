#include "header.h"
#include "coros/network/socket.h"
#include "coros_fcgi/commons/integer.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

coros::fcgi::RecordHeader::RecordHeader() {
}

coros::fcgi::RecordHeader::RecordHeader(ProtocolVersion version, RecordType type, 
                                        int request_id, int content_length) {
    this->version = version;
    this->type = type;
    this->request_id = request_id;
    this->content_length = content_length;
    int remainder = (content_length + FCGI_HEADER_LEN) % FCGI_ALIGNMENT_LEN;
    if (remainder > 0) {
        this->padding_length = FCGI_ALIGNMENT_LEN - remainder;
    } else {
        this->padding_length = 0;
    }
}

coros::base::AwaitableFuture coros::fcgi::RecordHeader::parse(coros::base::Socket& socket) {
    uint8_t header_data[FCGI_HEADER_LEN];
    co_await socket.read(reinterpret_cast<std::byte*>(header_data), FCGI_HEADER_LEN);
    this->version = static_cast<ProtocolVersion>(header_data[0]);
    this->type = static_cast<RecordType>(header_data[1]);
    this->request_id = coros::fcgi::read_uint16_be(header_data + 2);
    this->content_length = coros::fcgi::read_uint16_be(header_data + 4);
    this->padding_length = header_data[6];
}

coros::base::AwaitableFuture coros::fcgi::RecordHeader::serialize(coros::base::Socket& socket) {
    uint8_t header_data[FCGI_HEADER_LEN];
    header_data[0] = this->version;
    header_data[1] = this->type;
    coros::fcgi::write_uint16_be(header_data + 2, this->request_id);
    coros::fcgi::write_uint16_be(header_data + 4, this->content_length);
    header_data[6] = this->padding_length;
    co_await socket.write(reinterpret_cast<std::byte*>(header_data), FCGI_HEADER_LEN);
}

coros::base::AwaitableFuture coros::fcgi::RecordHeader::pad(coros::base::Socket& socket) {
    for (int i = 0; i < padding_length; ++i) {
        co_await socket.write_b(std::byte{0});
    }
}
