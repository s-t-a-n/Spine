#pragma once

#include <optional>
#include <string_view>

namespace spn::io {

class BufferedStream;

/// A transaction in the form of a request response
class Transaction {
public:
    Transaction(const Transaction& other) = delete;
    Transaction(Transaction&& other) noexcept
        : _stream(other._stream), _incoming_line(std::move(other._incoming_line)),
          _outgoing_queued_bytes(other._outgoing_queued_bytes), _is_finalized(other._is_finalized) {
        other._stream = nullptr;
        other._is_finalized = true;
    }
    Transaction& operator=(const Transaction& other) = delete;
    Transaction& operator=(Transaction&& other) noexcept {
        if (this == &other) return *this;
        _stream = other._stream;
        other._stream = nullptr;
        _incoming_line = std::move(other._incoming_line);
        _outgoing_queued_bytes = other._outgoing_queued_bytes;
        _is_finalized = other._is_finalized;
        other._is_finalized = true;
        return *this;
    }
    explicit Transaction(BufferedStream* stream, std::optional<size_t> discovered_length = {});
    ~Transaction() {
        if (!is_finalized()) finalize(); // RAII
    };

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