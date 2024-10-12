#include "spine/io/stream/buffered_stream.hpp"

namespace spn::io {

BufferedStream::BufferedStream(std::shared_ptr<Stream> stream, const BufferedStream::Config&& cfg)
    : _cfg(cfg), _input_buffer(cfg.input_buffer_size, cfg.delimiters),
      _output_buffer(_cfg.output_buffer_size, cfg.delimiters), _stream(std::move(stream)) {}
size_t BufferedStream::buffered_write(uint8_t value, bool rollover) { return _output_buffer.push(value, rollover); }
size_t BufferedStream::buffered_write(const char* const buffer, size_t lenght, bool rollover) {
    return _output_buffer.push(buffer, lenght, rollover);
}

std::optional<std::string_view> BufferedStream::get_next_line_view(const std::optional<size_t>& discovered_length) {
    return _input_buffer.get_next_line_view(discovered_length);
}

size_t BufferedStream::get_next_line(char* buffer, size_t max_length, const std::optional<size_t>& discovered_length) {
    return _input_buffer.get_next_line(buffer, max_length, discovered_length);
}

bool BufferedStream::drop_next_line(const std::optional<size_t>& discovered_length) {
    return _input_buffer.drop_next_line(discovered_length);
}

size_t BufferedStream::length_of_next_line() const { return _input_buffer.length_of_next_line(); }
size_t BufferedStream::pull_in_data() {
    size_t bytes_read = 0;
    uint8_t last_char = '\0';

    while (_stream->available() > 0) {
        // early break when a delimiter has been found, as to lose no input if possible
        if (_input_buffer.full()) {
            if (_input_buffer.delimiters().find(last_char) != std::string::npos)
                break; // break when the last incoming was a delimiter
            if (_input_buffer.overrun_space() == 0 && _input_buffer.has_line())
                break; // break when the buffer just turned full and the buffer already contains a line
        }

        if (!_stream->read(last_char)) {
            return bytes_read; // read failure
        }
        _input_buffer.push(last_char, true);
        ++bytes_read;
    }
    return bytes_read;
}

size_t BufferedStream::push_out_data() {
    size_t bytes_written = 0;

    while (_stream->available_for_write() > 0) {
        char v;
        if (!_output_buffer.pop(v) || !_stream->write(v)) return bytes_written;
        ++bytes_written;
    }
    return bytes_written;
}

std::optional<Transaction> BufferedStream::new_transaction() {
    const auto discovered_length = length_of_next_line();

    return length_of_next_line() > 0 ? std::make_optional(Transaction(this, discovered_length)) : std::nullopt;
}

} // namespace spn::io