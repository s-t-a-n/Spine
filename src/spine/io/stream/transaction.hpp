#pragma once

#include "spine/core/debugging.hpp"

#include <optional>
#include <string_view>
// #include "spine/io/stream/buffered_stream.hpp"

namespace spn::io {

class BufferedStream;

/// A transaction in the form of a request response
class Transaction {
public:
    Transaction(BufferedStream* stream, std::optional<size_t> discovered_length = {});

    /// Returns a string_view of the next incoming line in the buffer that is the cause of this transaction
    const std::string_view& incoming() const;

    /// Push a byte in the outgoing queue
    size_t outgoing(char value);
    size_t outgoing(const char* buffer, size_t length);
    size_t outgoing(const std::string_view& view) { return outgoing(view.data(), view.size()); }

    /// Returns the amount of space left for a reply
    size_t outgoing_space_left() const;

    /// Returns the amount of space currently queued up to reply
    size_t outgoing_space_used() const;

    /// Commit the transaction
    void commit();

    /// Abort the transaction by dropping queued outgoing bytes
    void abort();

    /// Returns true if transaction was committed or aborted
    bool is_finalized() const;

protected:
    /// Finalizes this transaction by dropping the incoming line and comitting all queued bytes
    void finalize();

    /// Undo the last `n_bytes` queued up for a reply or all queued bytes if 'n_bytes' is left at 0
    void undo(size_t n_bytes = 0) const;

private:
    BufferedStream* _stream;

    std::string_view _incoming_line;
    size_t _outgoing_queued_bytes{0};

    bool _is_finalized = false;
};

} // namespace spn::io