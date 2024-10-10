#pragma once

#include "spine/core/debugging.hpp"
// #include "spine/platform/hal.hpp"
#include "spine/structure/linebuffer.hpp"

#include <memory>
#include <optional>

namespace spn::io {

/// A basic stream
class Stream {
public:
    virtual ~Stream() = default;
    virtual void initialize() = 0;

    size_t read(char* const buffer, size_t length) { return read((uint8_t*)buffer, length); };
    virtual size_t read(uint8_t* buffer, size_t length) {
        spn_assert(!"Virtual base function called");
        return -1;
    };
    virtual bool read(uint8_t& value) {
        spn_assert(!"Virtual base function called");
        return false;
    };

    virtual size_t available() const {
        spn_assert(!"Virtual base function called");
        return -1;
    };
    virtual size_t available_for_write() const {
        spn_assert(!"Virtual base function called");
        return -1;
    };

    size_t write(const std::string_view& view) { return write((uint8_t*)view.data(), view.length()); }
    size_t write(const char* const buffer, size_t length) { return write((uint8_t*)buffer, length); };
    bool write(const char value) { return write(static_cast<uint8_t>(value)); }

    virtual size_t write(const uint8_t* const buffer, size_t length) {
        spn_assert(!"Virtual base function called");
        return -1;
    };
    virtual bool write(const uint8_t value) {
        spn_assert(!"Virtual base function called");
        return false;
    };

    virtual void flush() { spn_assert(!"Virtual base function called"); }
};

} // namespace spn::io
