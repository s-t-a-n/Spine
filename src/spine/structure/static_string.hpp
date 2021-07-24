#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/utils/string.hpp"

#include <cstring>
#include <string>
#include <string_view>

namespace kaskas::prompt {

/// String wrapper for a staticly allocated string with some C++23 sugar
class StaticString {
public:
    explicit StaticString(size_t buffer_length) : _buffer(buffer_length, '\0') { _buffer.resize(capacity()); }
    StaticString& operator=(const StaticString& other) = delete;
    StaticString& operator=(StaticString&& other) = delete;

    size_t size() const { return _buffer.size(); }

    /// Tell internal std::string it's size. Pay attention
    void set_size(size_t size) {
        DBG("ssssiiize : %i, cap: %i", size, capacity());
        assert(size <= capacity() && "StaticString: tried to reallocate");
        _buffer.resize(std::min(size, capacity()));
    }

    size_t capacity() const { return _buffer.capacity(); }

    const char* c_str() const { return _buffer.c_str(); }

    /// Returns data pointer. When writing to this pointer, make sure to announce the size through
    /// `expected_bytes_written`. See comment below.
    char* data(const size_t expected_bytes_written = 0) {
        // because std::string::resize() truncates bytes when upsizing, do this first
        // a known limitation with a workaround in c++23: https://stackoverflow.com/a/74655727
        // when 23 is around, the 'expected_bytes_written' can be dropped because truncation will only happen in
        // 'string::resize_and_overwrite', no longer in 'string::resize'
        if (expected_bytes_written > 0) {
            DBG("data: expected: %i", expected_bytes_written);
            set_size(expected_bytes_written);
        }

        return _buffer.data();
    }
    std::string_view view() const { return {_buffer.data(), _buffer.size()}; }

public:
    // todo : these are duplicates from core/utils/string
    StaticString& lstrip(std::string_view delimiters = " \f\n\r\t\v") {
        auto it = _buffer.begin();
        while (delimiters.find(*it) != std::string::npos)
            ++it;
        const auto len = it - _buffer.begin();
        std::memmove(data(), data() + len, size() - len);
        set_size(size() - len);
        return *this;
    }

    //
    StaticString& rstrip(std::string_view delimiters = " \f\n\r\t\v") {
        auto it = _buffer.rbegin();
        while (delimiters.find(*it) != std::string::npos)
            ++it;
        const auto len = it - _buffer.rbegin();
        set_size(_buffer.size() - len);
        return *this;
    }

    bool starts_with(const std::string_view needle) const { return _buffer.compare(0, needle.size(), needle, 0) == 0; }
    bool ends_with(const std::string_view needle) const {
        return _buffer.compare(_buffer.size() - needle.size(), _buffer.size(), needle) == 0;
    }

private:
    std::string _buffer;
};

} // namespace kaskas::prompt