#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/utils/string.hpp"

#include <cstring>
#include <string>
#include <string_view>

namespace spn::structure {

/// String wrapper for a statically allocated string with some C++23 sugar
class StaticString {
public:
    explicit StaticString(size_t buffer_length);
    StaticString& operator=(const StaticString& other) = delete;
    StaticString& operator=(StaticString&& other) = delete;

    /// Returns the size of the string (this is not the same as the capacity!).
    size_t size() const { return _buffer.size(); }

    /// Set the size of the string (this is not the same as the capacity!).
    void set_size(size_t size);

    /// Returns the total memory space allocated for this string.
    size_t capacity() const { return _buffer.capacity(); }

    /// Returns data pointer. When writing to this pointer, make sure to announce the size through
    /// `expected_bytes_written`. See comment below.
    char* data(const size_t expected_bytes_written = 0);

    /// Return C style string.
    const char* c_str() const { return _buffer.c_str(); }

    /// Returns a std::string_view of the string.
    std::string_view view() const { return {_buffer.data(), _buffer.size()}; }

    // todo : these are duplicates from core/utils/string

    /// Strip the characters specified by `delimiters` from the front of the string.
    StaticString& lstrip(std::string_view delimiters = " \f\n\r\t\v");

    /// Strip the characters specified by `delimiters` from the back of the string.
    StaticString& rstrip(std::string_view delimiters = " \f\n\r\t\v");

    /// Returns true if the string starts with `needle`
    bool starts_with(const std::string_view needle) const;

    /// Returns true if the string ends with `needle`
    bool ends_with(const std::string_view needle) const;

private:
    std::string _buffer;
};

} // namespace spn::structure
