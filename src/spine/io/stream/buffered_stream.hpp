#pragma once

#include "spine/io/stream/stream.hpp"
#include "spine/io/stream/transaction.hpp"
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
    explicit BufferedStream(std::shared_ptr<Stream> stream, const Config&& cfg);

    size_t buffered_write(const std::string_view& view) { return buffered_write(view.data(), view.size()); }
    size_t buffered_write(uint8_t value, bool rollover = false);
    size_t buffered_write(const char* const buffer, size_t lenght, bool rollover = false);

    /// Returns true if a delimited line was found
    bool has_line() const { return _input_buffer.length_of_next_line() > 0; }

    /// Returns a std::string_view of the next available line or nullopt if no line is present in the buffer.
    std::optional<std::string_view> get_next_line_view(const std::optional<size_t>& discovered_length = std::nullopt);

    /// Writes the next available line into `buffer` and returns the amount of bytes read
    size_t get_next_line(char* buffer, size_t max_length,
                         const std::optional<size_t>& discovered_length = std::nullopt);

    /// Drop the next available line from the buffer. Returns `true` when a message was succesfully dropped
    bool drop_next_line(const std::optional<size_t>& discovered_length = std::nullopt);

    /// Returns length of line if found or std::string::npos
    size_t length_of_next_line() const;

    /// Populate the input buffer. Returns the amount of bytes read into the buffer.
    size_t pull_in_data();

    /// Flush the output buffer. Returns the amount of bytes written from the buffer into the underlying stream.
    size_t push_out_data();

    std::optional<Transaction> new_transaction();

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
