#pragma once

#include "spine/core/debugging.hpp"
#include "spine/io/stream/stream.hpp"
#include "spine/io/stream/transaction.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/linebuffer.hpp"

#include <memory>
#include <optional>

namespace spn::io {

class BufferedStream {
public:
    using Transaction = spn::io::Transaction;
    struct Config {
        size_t input_buffer_size = 1;
        size_t output_buffer_size = 1;
        std::string_view delimiters = "\r\n";
    };

public:
    explicit BufferedStream(std::shared_ptr<Stream> stream, const Config&& cfg)
        : _cfg(cfg), _input_buffer(cfg.input_buffer_size, cfg.delimiters),
          _output_buffer(_cfg.output_buffer_size, cfg.delimiters), _stream(std::move(stream)) {}

    size_t buffered_write(const std::string_view& view) { return buffered_write(view.data(), view.size()); }
    size_t buffered_write(uint8_t value, bool rollover = false) { return _output_buffer.push(value, rollover); }
    size_t buffered_write(const char* const buffer, size_t lenght, bool rollover = false) {
        return _output_buffer.push(buffer, lenght, rollover);
    }
    // size_t buffered_write(const uint8_t* const buffer, size_t lenght, bool rollover = false) {
    // return buffered_write(reinterpret_cast<const char* const>(buffer), lenght, rollover);
    // }

    /// Returns true if a delimited line was found
    bool has_line() const { return _input_buffer.length_of_next_line() > 0; }

    /// Returns a std::string_view of the next available line or nullopt if no line is present in the buffer.
    std::optional<std::string_view> get_next_line_view(std::optional<size_t> discovered_length = std::nullopt) {
        return _input_buffer.get_next_line_view(discovered_length);
    }

    /// Writes the next available line into `buffer` and returns the amount of bytes read
    size_t get_next_line(char* buffer, size_t max_length, std::optional<size_t> discovered_length = std::nullopt) {
        return _input_buffer.get_next_line(buffer, max_length, discovered_length);
    }

    /// Drop the next available line from the buffer. Returns `true` when a message was succesfully dropped
    bool drop_next_line(std::optional<size_t> discovered_length = std::nullopt) {
        return _input_buffer.drop_next_line(discovered_length);
    }

    /// Returns length of line if found or std::string::npos
    size_t length_of_next_line() const { return _input_buffer.length_of_next_line(); }

    /// Populate the input buffer. Returns the amount of bytes read into the buffer.
    size_t pull_in_data() {
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

    /// Flush the output buffer. Returns the amount of bytes written from the buffer into the underlying stream.
    size_t push_out_data() {
        size_t bytes_written = 0;

        while (_stream->available_for_write() > 0) {
            char v;
            if (!_output_buffer.pop(v) || !_stream->write(v)) return bytes_written;
            ++bytes_written;
        }
        return bytes_written;
    }

    std::optional<Transaction> new_transaction() {
        const auto discovered_length = length_of_next_line();

        return length_of_next_line() > 0 ? std::make_optional(Transaction(this, discovered_length)) : std::nullopt;
    }

    size_t input_buffer_space_left() const { return _input_buffer.free_space(); }
    size_t input_buffer_space_used() const { return _input_buffer.used_space(); }
    size_t output_buffer_space_left() const { return _output_buffer.free_space(); }
    size_t output_buffer_space_used() const { return _output_buffer.used_space(); }

    size_t output_buffer_drop_last(const size_t n) { return _output_buffer.drop_last(n); }
    size_t output_buffer_drop_first(const size_t n) { return _output_buffer.drop_first(n); }

private:
    Config _cfg;
    structure::LineBuffer _input_buffer;
    structure::LineBuffer _output_buffer;

    std::shared_ptr<Stream> _stream;
};

} // namespace spn::io
