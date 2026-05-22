#ifndef SAFE_READ_UINT8_H
#define SAFE_READ_UINT8_H

#include <cstdint>
#include <optional>

namespace assignment_09 {

    std::optional<uint8_t> safe_read_uint8(const uint8_t* ptr);

}

#endif
