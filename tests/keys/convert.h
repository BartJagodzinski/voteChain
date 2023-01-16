#ifndef CONVERT_H
#define CONVERT_H
#include <sstream>
#include <vector>
#include <iomanip>

namespace convert {

    std::string uint8_to_hex_string(const uint8_t *data, const size_t size) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < size; ++i)
            ss << std::hex << std::setw(2) << static_cast<int>(data[i]);
        return ss.str();
    }
}

#endif
