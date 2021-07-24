#pragma once

#include "spine/structure/ringbuffer.hpp"

#include <optional>
#include <string>

namespace spn::structure {

class LineBuffer : public RingBuffer<char> {
public:
    LineBuffer(size_t capacity, const std::string_view delimiters = "\r\n")
        : RingBuffer<char>(capacity), _delimiters(delimiters) {}

    /// Push a string_view into the linebuffer
    size_t push(const std::string_view& buffer) { return RingBuffer<char>::push(buffer.data(), buffer.size()); }
    using RingBuffer::push;

    /// Set's the delimiters that determine a line
    void set_delimiters(const std::string_view delimiters = "\r\n") { _delimiters = delimiters; }

    /// The delimiters that determine a line
    const std::string_view& delimiters() const { return _delimiters; }

    /// Returns true if a delimited line was found
    bool has_line() const { return length_of_next_line() > 0; }

    /// Returns a std::string_view of the next available line or nullopt if no line is present in the buffer.
    std::optional<std::string_view> get_next_line_view(const std::optional<size_t>& discovered_length = std::nullopt) {
        auto length = discovered_length.value_or(length_of_next_line());
        if (length == 0) return {};
        length -= 1; // ignore delimiter
        char* str;
        if (peek_at(&str, 0) == false) return std::nullopt;
        return std::string_view(str, length);
    }

    /// Writes the next available line into `buffer` and returns the amount of bytes read
    size_t get_next_line(char* buffer, size_t max_length,
                         const std::optional<size_t>& discovered_length = std::nullopt) { //
        auto length = discovered_length.value_or(length_of_next_line());
        if (length == 0 || length > max_length) return 0;
        length -= 1; // ignore delimiter
        for (size_t i = 0; i < length; ++i) {
            pop(buffer[i]);
        }
        pop(); // get rid off delimiter
        buffer[length] = '\0'; // guarantee null determination
        return length;
    }

    /// Drop the next available line from the buffer. Returns `true` when a message was succesfully dropped
    bool drop_next_line(const std::optional<size_t>& discovered_length = std::nullopt) {
        auto length = discovered_length.value_or(length_of_next_line());
        if (length == 0) return false;
        length -= 1; // ignore delimiter
        for (size_t i = 0; i < length; ++i) {
            char v;
            pop(v);
        }
        pop(); // get rid of delimiter
        return true;
    }

    /// Returns length of line if found or 0
    size_t length_of_next_line() const {
        size_t length = 0;
        char v;

        for (size_t i = 0; i < used_space(); ++i) {
            if (!peek_at(v, i))
                return 0; // todo: this is problematic as there is no way to distinguish between no line and a read
                          // error
            ++length;
            if (_delimiters.find(v) != std::string::npos) return length;
        }
        return 0;
    }

private:
    std::string_view _delimiters{"\r\n"};
};

} // namespace spn::structure
