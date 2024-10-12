#include "spine/structure/linebuffer.hpp"

namespace spn::structure {

LineBuffer::LineBuffer(size_t capacity, const std::string_view delimiters)
    : RingBuffer<char>(capacity), _delimiters(delimiters) {}

size_t LineBuffer::push(const std::string_view& buffer) { return RingBuffer<char>::push(buffer.data(), buffer.size()); }

void LineBuffer::set_delimiters(const std::string_view delimiters) { _delimiters = delimiters; }

std::optional<std::string_view> LineBuffer::get_next_line_view(const std::optional<size_t>& discovered_length) {
    auto length = discovered_length.value_or(length_of_next_line());
    if (length == 0) return {};
    length -= 1; // ignore delimiter
    char* str;
    if (peek_at(&str, 0) == false) return std::nullopt;
    return std::string_view(str, length);
}

size_t LineBuffer::get_next_line(char* buffer, size_t max_length, const std::optional<size_t>& discovered_length) { //
    auto length = discovered_length.value_or(length_of_next_line());
    if (length == 0 || length > max_length) return 0;
    length -= 1; // ignore delimiter
    for (size_t i = 0; i < length; ++i) {
        pop(buffer[i]);
    }
    pop(); // get rid of delimiter
    buffer[length] = '\0'; // guarantee null determination
    return length;
}

bool LineBuffer::drop_next_line(const std::optional<size_t>& discovered_length) {
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

size_t LineBuffer::length_of_next_line() const {
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

} // namespace spn::structure