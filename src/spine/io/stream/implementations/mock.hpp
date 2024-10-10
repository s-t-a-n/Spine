#pragma once

#include "spine/core/debugging.hpp"
#include "spine/io/stream/stream.hpp"
#include "spine/structure/ringbuffer.hpp"

#include <vector>

namespace spn::io {

class MockStream final : public Stream {
public:
    struct Config {
        size_t input_buffer_size;
        size_t output_buffer_size;
    };

    MockStream(const Config&& cfg)
        : Stream(), _cfg(cfg), _stdin(_cfg.input_buffer_size), _stdout(_cfg.output_buffer_size), _active_in(&_stdin),
          _active_out(&_stdout){};
    ~MockStream() override = default;

public:
    void initialize() override {}

    void inject_bytestream(const std::vector<uint8_t>& byte_stream) {
        swap_streams();
        write(byte_stream.data(), byte_stream.size());
        swap_streams();
    }

    std::optional<std::vector<uint8_t>> extract_bytestream() {
        swap_streams(); // swap input and outputbuffer
        const auto bytes_to_read = available();
        auto byte_stream = std::vector<uint8_t>();
        byte_stream.reserve(bytes_to_read); // allocate enough
        byte_stream.resize(bytes_to_read); // until c++23, this is stop truncation
        byte_stream.resize(read(byte_stream.data(), bytes_to_read)); // resize to bytes actually read
        byte_stream.shrink_to_fit(); // trim off the fat
        swap_streams(); // swap the streams back around
        return byte_stream;
    }

    using Stream::write;
    bool write(const uint8_t value) override { return _active_out->push(value); }
    size_t write(const uint8_t* const buffer, size_t length) override { return _active_out->push(buffer, length); }

    using Stream::read;
    bool read(uint8_t& value) override { return _active_in->pop(value); }
    size_t read(uint8_t* buffer, size_t length) override { return _active_in->pop(buffer, length); }

    size_t available() const override { return _active_in->used_space(); }
    size_t available_for_write() const override { return _active_out->free_space(); }
    void flush() override {}

protected:
    void swap_streams() {
        spn_assert(_active_in != nullptr);
        spn_assert(_active_out != nullptr);
        std::swap(_active_in, _active_out);
    }

private:
    Config _cfg;

    structure::RingBuffer<uint8_t> _stdin;
    structure::RingBuffer<uint8_t> _stdout;

    structure::RingBuffer<uint8_t>* _active_in;
    structure::RingBuffer<uint8_t>* _active_out;
};
} // namespace spn::io
