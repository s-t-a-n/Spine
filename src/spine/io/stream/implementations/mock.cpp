#include "spine/io/stream/implementations/mock.hpp"

namespace spn::io {

MockStream::MockStream(const MockStream::Config&& cfg)
    : Stream(), _cfg(cfg), _stdin(_cfg.input_buffer_size), _stdout(_cfg.output_buffer_size), _active_in(&_stdin),
      _active_out(&_stdout) {}

void MockStream::inject_bytestream(const std::vector<uint8_t>& byte_stream) {
    swap_streams();
    write(byte_stream.data(), byte_stream.size());
    swap_streams();
}
std::optional<std::vector<uint8_t>> MockStream::extract_bytestream() {
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
void MockStream::swap_streams() {
    spn_assert(_active_in != nullptr);
    spn_assert(_active_out != nullptr);
    std::swap(_active_in, _active_out);
}
} // namespace spn::io