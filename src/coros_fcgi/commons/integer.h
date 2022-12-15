#ifndef COROS_FCGI_COMMONS_INTEGER_H
#define COROS_FCGI_COMMONS_INTEGER_H

#include <cstdint>

namespace coros::fcgi {
    uint16_t read_uint16_be(uint8_t* data);

    uint32_t read_uint32_be(uint8_t* data);
}

#endif
