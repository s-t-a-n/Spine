#pragma once

#include "spine/core/debugging.hpp"
#include "spine/structure/array.hpp"

namespace spn::structure {

template<typename T>
class RingBuffer {
public:
    RingBuffer(size_t capacity, T init_value = {}) : m_buffer(capacity, init_value) { assert(capacity > 0); }

    size_t push(const T& value, bool rollover = false) {
        if (m_is_full && !rollover) return false;
        m_buffer[m_head] = value;
        m_head = (m_head + 1) % m_buffer.size();
        if (m_head == m_tail) {
            m_is_full = true;
        } else if (m_is_full) {
            m_tail = (m_tail + 1) % m_buffer.size();
            ++m_overwritten;
        }
        return true;
    }
    size_t push(const T* buffer, size_t length, bool rollover = false) {
        size_t values_written = 0;
        for (size_t i = 0; i < length; ++i) {
            if (!push(buffer[i], rollover)) return values_written;
            ++values_written;
        }
        return values_written;
    }

    bool pop() {
        T v;
        return pop(v);
    }
    bool pop(T& value) {
        if (empty()) return false;

        value = m_buffer[m_tail];
        m_tail = (m_tail + 1) % m_buffer.size();
        m_is_full = false;
        m_overwritten = 0;
        return true;
    }

    size_t pop(T* buffer, size_t length) {
        size_t bytes_read = 0;

        if (used_space() < length) return 0;

        for (size_t i = 0; i < length; ++i) {
            pop(buffer[i]);
            ++bytes_read;
        }

        return bytes_read;
    }

    /// Get a value from the buffer without removing it
    bool peek_at(T& value, size_t index) const {
        if (index >= used_space()) return false;

        auto tail = (m_tail + index) % m_buffer.size();
        value = m_buffer[tail];
        return true;
    }

    /// Get a string_view from the buffer without removing it
    bool peek_at(T** ptr, size_t index) {
        if (index >= used_space() || !ptr) return false;
        auto tail = (m_tail + index) % m_buffer.size();
        *ptr = &m_buffer[tail];
        return true;
    }

    /// Drop `n_last` inserted elements of the buffer or all if left at 0. Returns amount of dropped elements.
    size_t drop_last(size_t n = 0) {
        n = std::min(used_space(), n);
        retract_head(n);

        m_is_full = false;
        m_overwritten = std::max(static_cast<size_t>(0), m_overwritten - n);
        return n;
    }

    /// Drop `n_last` inserted elements of the buffer or all if left at 0. Returns amount of dropped elements.
    size_t drop_first(size_t n = 0) {
        n = std::min(used_space(), n);
        advance_tail(n);

        m_is_full = false;
        m_overwritten = std::min(m_overwritten, used_space() - n);
        return n;
    }

    /// Drop all buffer elements
    void clear() { drop_last(used_space()); }

    bool empty() const { return !full() && m_head == m_tail; }
    bool full() const { return m_is_full; }

    /// Amount of bytes that were overwritten without being read (due to overflow)
    size_t overrun_space() const { return m_overwritten; }

    /// Amount of bytes occupied in the buffer
    size_t used_space() const {
        if (full()) return capacity();
        if (m_head >= m_tail) return m_head - m_tail;
        return capacity() - m_tail + m_head;
    }

    /// Amount of bytes ready to be written into the buffer
    size_t free_space() const {
        if (full()) return 0;
        if (m_head >= m_tail) return capacity() - m_head + m_tail;
        return m_tail - m_head;
    }

    /// Total amount of bytes that the buffer can hold without overrun
    size_t capacity() const { return m_buffer.max_size(); }

protected:
    /// Push the reading head forwards by `n`, effectively dropping `n` first inserted  elements
    void advance_tail(size_t n) { m_tail = (m_tail + n) % m_buffer.size(); }

    /// Pull the writing head backwards by `n`, effectively dropping `n` last inserted  elements
    void retract_head(size_t n) { m_head = (m_head + m_buffer.size() - (n % m_buffer.size())) % m_buffer.size(); }

private:
    Array<T> m_buffer;
    size_t m_head{0};
    size_t m_tail{0};

    size_t m_overwritten{0};
    bool m_is_full = false; // needed because otherwise empty and full cannot be distinguished from each other
};

} // namespace spn::structure
