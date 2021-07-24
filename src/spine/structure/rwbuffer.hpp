#pragma once

#include "spine/platform/hal.hpp"
#include "spine/structure/array.hpp"

namespace spn::structure {

/// todo: DEPRECATED in favour of ringbuffer!
/// provides intermediary storage between writing and reading of data
template<typename T = uint8_t, typename Size = uint16_t>
class RWBuffer {
public:
    using Internal = nullptr_t;

    RWBuffer(T* buffer, Size buffer_length) {
        if (buffer == nullptr) {
            _buffer = new T[buffer_length];
            _internal = true;
        } else {
            _buffer = buffer;
            _internal = false;
        }
        _size = buffer_length;
    }
    ~RWBuffer() {
        if (_internal) delete[] _buffer;
    }

public:
    Size read(T* buffer, Size length) {
        auto read = std::min(length, available_to_read());
        memcpy(buffer, _buffer + _head, read * sizeof(T));
        _head += read;
        if (_head == _tail) {
            _head = 0;
            _tail = 0;
        }
        return read;
    }

    T read() {
        assert(available_to_read() > 0);
        const auto v = _buffer[_head++];
        if (_head == _tail) {
            _head = 0;
            _tail = 0;
        }
        return v;
    }

    Size write(T* buffer, Size length) {
        if (length > available_to_write() && length < _size - available_to_read()) {
            // move everything to front to make space
            memmove(_buffer, _buffer + _head, available_to_read() * sizeof(T));
            _tail = available_to_read();
            _head = 0;
        }

        auto written = std::min(length, available_to_write());
        memcpy(_buffer + _tail, buffer, written * sizeof(T));
        _tail += written;
        return written;
    }

    Size available_to_read() const { return _tail - _head; }
    Size available_to_write() const { return _size - _tail; }

    // protected:
    /// user is responsible for avoiding buffer overflows!
    T* raw_write_head() { return _buffer + _tail; }
    void raw_adjust_write_head(uint16_t offset) { _tail += offset; }

private:
    bool _internal;
    T* _buffer = nullptr;
    Size _size;
    Size _head = 0;
    Size _tail = 0;
};

} // namespace spn::structure