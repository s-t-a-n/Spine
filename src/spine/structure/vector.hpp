#pragma once

#include "spine/core/debugging.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/array.hpp"

// #if not defined(ARDUINO)
// #    include <cstring>
// #endif

namespace spn::structure {

/*******************************************************************************
** Vector
*******************************************************************************/

// Topographical layout
// m_head = 1
// m_tail = 4
// size() = 3
// 0      1  2  3  4  5  6
// m_head ^        ^ m_tail
//        ^  ^  ^ data
// m_tail always points to the location of last_element + 1

template<typename T>
class Vector : public Array<T> {
public:
    using Size = ArrayBase::Size;

    // Vector(EmptyArrayTag&&);

    Vector(Size max_size);
    // Vector(uint8_t* buffer, Size max_size);
    template<Size CAP>
    Vector(T (&store)[CAP], Size size = 0);
    Vector(Size max_size, T init_value);
    Vector(const std::initializer_list<T>& value_list);

    Vector& operator=(const std::initializer_list<T>& value_list) noexcept;

    T& operator[](Size index);
    const T& operator[](Size index) const;

    void push_back(const T& item);
    void emplace_back(const T&& item);

    void push_front(const T& item);
    void emplace_front(const T&& item);

    T& peek_back() const;
    [[nodiscard]] T&& pop_back();

    T& peek_front() const;
    [[nodiscard]] T&& pop_front();

    void insert(Size index, const T& item);
    T remove(Size index);

    void resize();

    void reserve(Size size);
    void shrink_to_fit();

    void clear();

    T& back();

    typename Array<T>::iterator begin();
    typename Array<T>::const_iterator begin() const;
    typename Array<T>::iterator end();
    typename Array<T>::const_iterator end() const;

    typename Array<T>::reverse_iterator rbegin();
    typename Array<T>::reverse_iterator rend();
    typename Array<T>::const_reverse_iterator rbegin() const;
    typename Array<T>::const_reverse_iterator rend() const;

    bool empty() const;
    bool full() const;
    Size size() const;

    void fill(const T& value);

private:
    /// Rearrange memory such that after rearranging, the elements start at `idx`
    void rearrange(Size idx) {
        assert(idx + size() < Array<T>::max_size());

        // todo: convert this brutal C-stuff into nice and fluffy CPP-stuff
        memmove((char*)&this->m_store[idx], (char*)&this->m_store[m_head], size() * sizeof(T));
        const auto new_tail = idx + size();
        const auto new_head = idx;
        // DBGF("memmoving to idx %i, head:%i, tail:%i, newhead:%i, newtail:%i, size:%i", idx, m_head, m_tail, new_head,
        //      new_tail, size());

        // todo, optimize: only wipe elements that overlapped
        // todo: convert this brutal C-stuff into nice and fluffy CPP-stuff
        memset((char*)&this->m_store[0], '\0', new_head * sizeof(T)); // wipe all elements before `new_head`
        assert(size() - new_tail < this->max_size());
        memset((char*)&this->m_store[new_tail], '\0',
               (this->max_size() - new_tail) * sizeof(T)); // wipe all elements after `new_tail`

        m_head = new_head;
        m_tail = new_tail;
    }

private:
    Size m_head = 0;
    Size m_tail = 0;
};

/*******************************************************************************
** Vector Definitions
*******************************************************************************/

// template<typename T>
// Vector<T>::Vector(EmptyArrayTag&&) : Array<T>(EmptyArrayTag{}), m_head(0), m_tail(0) {}

// template<typename T>
// Vector<T>::Vector(uint8_t* buffer, ArrayBase::Size max_size) : Array<T>(buffer, max_size) {}

template<typename T>
Vector<T>::Vector(ArrayBase::Size max_size) : Array<T>(max_size) {}

template<typename T>
Vector<T>::Vector(ArrayBase::Size max_size, T init_value) : Array<T>(max_size, init_value), m_tail(max_size){};

template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& value_list) : Array<T>(value_list), m_tail(value_list.size()) {}

template<typename T>
Vector<T>& Vector<T>::operator=(const std::initializer_list<T>& value_list) noexcept {
    Array<T>::operator=(value_list);
    m_head = 0;
    m_tail = value_list.size();
    return *this;
}

template<typename T>
template<ArrayBase::Size CAP>
Vector<T>::Vector(T (&store)[CAP], ArrayBase::Size size) : Array<T>(store), m_tail(size) {}

template<typename T>
T& Vector<T>::operator[](ArrayBase::Size index) {
    assert(m_head + index < m_tail);
    return Array<T>::operator[](m_head + index);
}

template<typename T>
const T& Vector<T>::operator[](ArrayBase::Size index) const {
    assert(m_head + index < m_tail);
    return Array<T>::operator[](m_head + index);
}

template<typename T>
void Vector<T>::push_back(const T& item) {
    if (m_tail >= this->max_size() && m_head > 0) {
        // make space
        rearrange(0);
    }
    assert(m_tail < this->max_size());

    if (m_tail < this->max_size()) this->m_store[m_tail++] = item;
}

template<typename T>
void Vector<T>::emplace_back(const T&& item) {
    if (m_tail >= this->max_size() && m_head > 0) {
        // make space
        rearrange(0);
    }
    assert(m_tail < this->max_size());
    if (m_tail < this->max_size()) {
        assert(sizeof(item) == sizeof(T));
        this->m_store[m_tail++] = std::move(item);
    }
}

template<typename T>
T& Vector<T>::peek_back() const {
    assert(size() > 0);
    return (this->m_store[m_tail - 1]);
}

template<typename T>
[[nodiscard]] T&& Vector<T>::pop_back() {
    assert(size() > 0);
    if (m_tail > 0) {
        m_tail--;
        if (m_tail == m_head) {
            m_tail = 0;
            m_head = 0;
        }
    }
    return std::move(this->m_store[m_tail]);
}

template<typename T>
void Vector<T>::push_front(const T& item) {
    assert(this->size() < this->max_size());
    if (m_head > 0) {
        this->m_store[--m_head] = item;
        return;
    }
    // DBGF("push_front, size: %i, m_head: %i, m_tail: %i", size(), m_head, m_tail);
    //
    // for (auto v : *this) {
    //     HAL::print(v.use_count());
    // }
    // HAL::println("");

    // todo: use `rearrange()`
    // todo: convert this brutal C-stuff into nice and fluffy CPP-stuff
    memmove((char*)&this->m_store[m_head + 1], (char*)&this->m_store[m_head], size() * sizeof(T));
    memset((char*)&this->m_store[m_head], '\0',
           sizeof(T)); // otherwise copy assignment below interacts with moved memory

    this->m_store[m_head] = item;
    m_tail++;

    // for (auto v : *this) {
    //     HAL::print(v.use_count());
    // }
    // HAL::println("");
}

template<typename T>
void Vector<T>::emplace_front(const T&& item) {
    if (m_head > 0) {
        this->m_store[--m_head] = std::move(item);
        return;
    }

    // todo: convert this brutal C-stuff into nice and fluffy CPP-stuff
    memmove((char*)&this->m_store[m_head + 1], (char*)&this->m_store[m_head], size() * sizeof(T));
    memset((char*)&this->m_store[m_head], '\0',
           sizeof(T)); // otherwise copy assignment below interacts with moved memory
    this->m_store[m_head] = std::move(item);
    m_tail++;
}

template<typename T>
T& Vector<T>::peek_front() const {
    assert(size() > 0);
    return (this->m_store[m_head]);
}

template<typename T>
T&& Vector<T>::pop_front() {
    assert(size() > 0);
    const auto cur_head = m_head++;
    if (m_tail == m_head) {
        m_tail = 0;
        m_head = 0;
    }
    return std::move(this->m_store[cur_head]);
}

template<typename T>
void Vector<T>::insert(ArrayBase::Size index, const T& item) {
    assert(this->size() < this->m_max_size);

    if (index == 0) {
        push_front(item);
        return;
    }

    if (m_head + index >= m_tail) {
        push_back(item);
        return;
    }

    if (m_tail >= this->max_size() && m_head > 0) {
        // make space
        rearrange(0);
    }
    assert(m_tail < this->max_size());

    // DBGF("memmove target idx:%i, source idx: %i, length: %i, sizeof: %i", m_head + index + 1, m_head + index,
    //      (m_tail - (m_head + index)), sizeof(T));

    // DBGF("vector inserting @ %i, max_size: %i, size: %i m_head: %i, m_tail: %i", index, this->max_size(),
    // this->size(), m_head, m_tail);

    // todo: convert this brutal C-stuff into nice and fluffy CPP-stuff
    memmove((char*)&this->m_store[m_head + index + 1], (char*)&this->m_store[m_head + index],
            (m_tail++ - (m_head + index)) * sizeof(T));
    memset((char*)&this->m_store[m_head + index], '\0',
           sizeof(T)); // otherwise copy assignment below interacts with moved memory
    this->m_store[m_head + index] = item;
}

template<typename T>
T Vector<T>::remove(ArrayBase::Size index) {
    if (index == m_head) {
        return std::move(pop_front());
    }
    if (index == m_tail) {
        return std::move(pop_back());
    }
    if (index > m_head && index < m_tail) {
        auto removed = std::move(this->m_store[index]);
        memmove(&this->m_store[index], &this->m_store[index + 1], (m_tail-- - index) * sizeof(T));
        if (m_tail == m_head) {
            m_tail = 0;
            m_head = 0;
        }
        return std::move(removed);
    }
    assert(!"remove was called with illegal index");
    return {};
}

template<typename T>
void Vector<T>::clear() {
    // todo: inefficient
    for (auto& v : *this) {
        const auto _ = pop_back();
    }
}

template<typename T>
T& Vector<T>::back() {
    return this->m_store[m_tail > 0 ? m_tail - 1 : 0];
}

/* iterators */

template<typename T>
typename Array<T>::iterator Vector<T>::begin() {
    return typename Array<T>::iterator(this->m_store, m_head);
}

template<typename T>
typename Array<T>::const_iterator Vector<T>::begin() const {
    return typename Array<T>::const_iterator(this->m_store, m_head);
}

template<typename T>
typename Array<T>::iterator Vector<T>::end() {
    return typename Array<T>::iterator(this->m_store, m_tail);
}

template<typename T>
typename Array<T>::const_iterator Vector<T>::end() const {
    return typename Array<T>::const_iterator(this->m_store, m_tail);
}

/* reverse iterators */

template<typename T>
typename Array<T>::reverse_iterator Vector<T>::rbegin() {
    return typename Array<T>::reverse_iterator(this->m_store, m_tail - (m_tail != m_head));
}

template<typename T>
typename Array<T>::reverse_iterator Vector<T>::rend() {
    return typename Array<T>::reverse_iterator(this->m_store, m_head - (m_tail != m_head));
}

template<typename T>
typename Array<T>::const_reverse_iterator Vector<T>::rbegin() const {
    return typename Array<T>::const_reverse_iterator(this->m_store, m_tail - (m_tail != m_head));
}

template<typename T>
typename Array<T>::const_reverse_iterator Vector<T>::rend() const {
    return typename Array<T>::const_reverse_iterator(this->m_store, m_head - (m_tail != m_head));
}

/* utility functions */

template<typename T>
ArrayBase::Size Vector<T>::size() const {
    return m_tail - m_head;
}

template<typename T>
bool Vector<T>::empty() const {
    return size() == 0;
}

template<typename T>
bool Vector<T>::full() const {
    return size() == this->m_max_size;
}

template<typename T>
void Vector<T>::fill(const T& value) {
    Array<T>::fill(value);
    m_head = 0;
    m_tail = this->m_max_size;
}

} // namespace spn::structure
