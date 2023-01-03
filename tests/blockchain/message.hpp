// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. http://www.boost.org/LICENSE_1_0.txt

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <cstdio>
#include <cstdlib>
#include <cstring>

class Message {
public:
    static constexpr std::size_t header_length = 4;
    static constexpr std::size_t max_body_length = 2048;

    Message() : _body_lenght(0) {}
    const char* data() const { return _data; }
    char* data() { return _data; }
    std::size_t length() const { return header_length + _body_lenght; }
    const char* body() const { return _data + header_length; }
    char* body() { return _data + header_length; }
    std::size_t body_length() const { return _body_lenght; }

    void body_length(std::size_t new_length) {
        _body_lenght = new_length;
        if (_body_lenght > max_body_length) _body_lenght = max_body_length;
    }

    bool decode_header() {
        char header[header_length + 1] = "";
        std::strncat(header, _data, header_length);
        _body_lenght = std::atoi(header);
        if (_body_lenght > max_body_length) {
            _body_lenght = 0;
            return false;
        }
        return true;
    }

    void encode_header() {
        char header[header_length + 1] = "";
        std::sprintf(header, "%4d", static_cast<int>(_body_lenght));
        std::memcpy(_data, header, header_length);
    }

private:
    char _data[header_length + max_body_length];
    std::size_t _body_lenght;
};

#endif
