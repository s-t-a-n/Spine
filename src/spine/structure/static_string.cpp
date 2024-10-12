#include "spine/structure/static_string.hpp"

namespace spn::structure {

StaticString::StaticString(size_t buffer_length) : _buffer(buffer_length, '\0') { _buffer.resize(capacity()); }
void StaticString::set_size(size_t size) {
    spn_assert(size <= capacity()); // do not allow reallocation
    _buffer.resize(std::min<>(size, capacity()));
}
char* StaticString::data(const size_t expected_bytes_written) {
    // because std::string::resize() truncates bytes when upsizing, do this first
    // a known limitation with a workaround in c++23: https://stackoverflow.com/a/74655727
    // when 23 is around, the 'expected_bytes_written' can be dropped because truncation will only happen in
    // 'string::resize_and_overwrite', no longer in 'string::resize'
    if (expected_bytes_written > 0) set_size(expected_bytes_written);
    return _buffer.data();
}
StaticString& StaticString::lstrip(std::string_view delimiters) {
    auto it = _buffer.begin();
    while (delimiters.find(*it) != std::string::npos)
        ++it;
    const auto len = it - _buffer.begin();
    std::memmove(data(), data() + len, size() - len);
    set_size(size() - len);
    return *this;
}
StaticString& StaticString::rstrip(std::string_view delimiters) {
    auto it = _buffer.rbegin();
    while (delimiters.find(*it) != std::string::npos)
        ++it;
    const auto len = it - _buffer.rbegin();
    set_size(_buffer.size() - len);
    return *this;
}
bool StaticString::starts_with(const std::string_view needle) const {
    return _buffer.compare(0, needle.size(), needle, 0) == 0;
}
bool StaticString::ends_with(const std::string_view needle) const {
    return _buffer.compare(_buffer.size() - needle.size(), _buffer.size(), needle) == 0;
}
} // namespace spn::structure
