#pragma once

#include "spine/structure/ringbuffer.hpp"

#include <optional>
#include <string>

namespace spn::structure {

class LineBuffer : public RingBuffer<char> {
public:
    LineBuffer(size_t capacity, const std::string_view delimiters = "\r\n");

    /// Push a string_view into the linebuffer
    size_t push(const std::string_view& buffer);
    using RingBuffer::push;

    /// Set's the delimiters that determine a line
    void set_delimiters(const std::string_view delimiters = "\r\n");

    /// The delimiters that determine a line
    const std::string_view& delimiters() const { return _delimiters; }

    /// Returns true if a delimited line was found
    bool has_line() const { return length_of_next_line() > 0; }

    /// Returns a std::string_view of the next available line or nullopt if no line is present in the buffer.
    std::optional<std::string_view> get_next_line_view(const std::optional<size_t>& discovered_length = std::nullopt);

    /// Writes the next available line into `buffer` and returns the amount of bytes read
    size_t get_next_line(char* buffer, size_t max_length,
                         const std::optional<size_t>& discovered_length = std::nullopt);

    /// Drop the next available line from the buffer. Returns `true` when a message was succesfully dropped
    bool drop_next_line(const std::optional<size_t>& discovered_length = std::nullopt);

    /// Returns length of line if found or 0
    size_t length_of_next_line() const;

private:
    std::string_view _delimiters{"\r\n"};
};

} // namespace spn::structure
