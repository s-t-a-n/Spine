#include "spine/io/stream/transaction.hpp"

#include "spine/io/stream/buffered_stream.hpp"

namespace spn::io {
Transaction::Transaction(BufferedStream* stream, std::optional<size_t> discovered_length) : _stream(stream) {
    assert(_stream);
    const auto length = discovered_length ? *discovered_length : _stream->length_of_next_line();
    assert(length > 0); // A transaction shouldn't be started without a line ready to be read
    const auto next_line_view = _stream->get_next_line_view(length);
    assert(next_line_view); // catch this because it could get nasty if left unhandled
    if (next_line_view) _incoming_line = *next_line_view;
}
const std::string_view& Transaction::incoming() const {
    assert(!is_finalized());
    return _incoming_line;
}
size_t Transaction::outgoing(char value) {
    assert(!is_finalized());
    assert(outgoing_space_left() > 0);
    if (outgoing_space_left() == 0) return 0;
    const auto bytes_written = _stream->buffered_write(value);
    _outgoing_queued_bytes += bytes_written;
    return bytes_written;
}
size_t Transaction::outgoing(const char* buffer, size_t length) {
    assert(!is_finalized());
    assert(outgoing_space_left() > length); // todo: instead make it optional to flush out bytes in between
    length = std::min(outgoing_space_left(), length);
    size_t bytes_written = _stream->buffered_write(buffer, length);
    _outgoing_queued_bytes += bytes_written;
    return bytes_written;
}

size_t Transaction::outgoing_space_left() const { return _stream->output_buffer_space_left(); }
size_t Transaction::outgoing_space_used() const { return _outgoing_queued_bytes; }
void Transaction::commit() {
    assert(!is_finalized());
    _outgoing_queued_bytes = 0; // effectively commit outgoing bytes;
    finalize();
}
void Transaction::abort() {
    assert(!is_finalized());
    undo();
    finalize();
}
void Transaction::finalize() {
    assert(!is_finalized());
    if (!is_finalized()) {
        _stream->drop_next_line(); // remove the line from the incoming buffer
        _incoming_line = {};
        _outgoing_queued_bytes = 0;
        _is_finalized = true;
    }
}
bool Transaction::is_finalized() const { return _is_finalized; }
void Transaction::undo(size_t n_bytes) const {
    n_bytes = n_bytes > 0 ? std::min(n_bytes, _outgoing_queued_bytes) : _outgoing_queued_bytes;
    _stream->output_buffer_drop_last(n_bytes);
}

} // namespace spn::io