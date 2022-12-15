#include "integer.h"

#include <cstdint>

uint16_t coros::fcgi::read_uint16_be(uint8_t* data) {
    uint16_t result = *data;
    result = (result << 8) + *(data + 1);
    return result;
}

uint32_t coros::fcgi::read_uint32_be(uint8_t* data) {
    uint32_t result = read_uint16_be(data);
    result = (result << 16) + read_uint16_be(data + 2);
    return result;
}