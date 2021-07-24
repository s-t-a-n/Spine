#pragma once

#include "spine/core/debugging.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

namespace spn::structure {

class ArrayBase {
public:
    // using Size = ptrdiff_t;
    using Size = unsigned long int;
};

/*******************************************************************************
** Iterators
*******************************************************************************/

template<typename T>
class ArrayIterator {
public:
    ArrayIterator();
    ArrayIterator(T* store_ptr, ArrayBase::Size index = 0);

    bool operator!=(const ArrayIterator<T>& other) const;
    bool operator==(const ArrayIterator<T>& other) const;
    ArrayIterator& operator++();
    ArrayIterator operator++(int);
    ArrayIterator& operator--();
    ArrayIterator operator--(int);
    T& operator*() const;

protected:
    T* m_ptr;
    ArrayBase::Size m_index;
};

template<typename T>
class ArrayReverseIterator : protected ArrayIterator<T> {
public:
    using ArrayIterator<T>::ArrayIterator;

    bool operator!=(const ArrayReverseIterator<T>& other) const;
    bool operator==(const ArrayReverseIterator<T>& other) const;
    ArrayReverseIterator& operator++();
    ArrayReverseIterator operator++(int);
    ArrayReverseIterator& operator--();
    ArrayReverseIterator operator--(int);
    using ArrayIterator<T>::operator*;
};

/*******************************************************************************
** Array
*******************************************************************************/

// struct EmptyArrayTag {};

template<typename T>
class Array : public ArrayBase {
public:
    // static_assert(std::is_copy_constructible_v<T>);
    // static_assert(std::is_constructible_v<T>);

public:
    // Array(EmptyArrayTag&&);

    Array(Size max_size);
    Array(uint8_t* buffer, Size buffer_length);
    Array(Size max_size, T init_value);
    Array(const std::initializer_list<T>& init_values);
    template<Size CAP>
    Array(T (&store)[CAP]);
    ~Array();
    Array(const Array& other);
    Array(Array&& other) noexcept;
    Array& operator=(const Array& other);
    Array& operator=(Array&& other) noexcept;
    Array& operator=(const std::initializer_list<T>& value_list) noexcept;

    using iterator = ArrayIterator<T>;
    using const_iterator = ArrayIterator<const T>;
    using reverse_iterator = ArrayReverseIterator<T>;
    using const_reverse_iterator = ArrayReverseIterator<const T>;

    // template<Size CAP>
    // void setBackingStore(T (&store)[CAP]);

    T& at(Size index) const;
    T& operator[](Size index);
    const T& operator[](Size index) const;
    T& front();
    T& back();
    T* data();
    const T* data() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    reverse_iterator rend();

    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    [[nodiscard]] bool empty() const;
    [[nodiscard]] Size size() const;
    [[nodiscard]] Size max_size() const;

    void swap(Array& other) noexcept;
    void fill(const T& value);

protected:
    T* m_store;
    Size m_max_size;

private:
    bool m_store_is_internal;
};

/*******************************************************************************
** Array Definitions
*******************************************************************************/

// template<typename T>
// Array<T>::Array(EmptyArrayTag&&) : m_store(nullptr), m_max_size(0), m_store_is_internal(false) {}

template<typename T>
template<ArrayBase::Size CAP>
Array<T>::Array(T (&store)[CAP]) : m_store(store), m_max_size(CAP), m_store_is_internal(false) {}

template<typename T>
Array<T>::Array(uint8_t* buffer, Size buffer_length)
    : m_store(static_cast<T*>(buffer)), m_max_size(buffer_length / sizeof(T)), m_store_is_internal(false) {
    memset(m_store, '\0', m_max_size * sizeof(T));
}

template<typename T>
Array<T>::Array(Size max_size)
    : m_store(static_cast<T*>(std::malloc(max_size * sizeof(T)))), m_max_size(max_size), m_store_is_internal(true) {
    memset((char*)m_store, '\0', m_max_size * sizeof(T));
}

template<typename T>
Array<T>::Array(Size max_size, T init_value) : Array(max_size) {
    fill(init_value);
}

template<typename T>
Array<T>::Array(const std::initializer_list<T>& init_values) : Array(init_values.size()) {
    assert(max_size() == init_values.size());
    auto i = 0;
    for (auto& obj : init_values) {
        m_store[i++] = obj;
    }
}

template<typename T>
Array<T>::~Array() {
    if (m_store_is_internal) {
        std::free(m_store);
        m_store = nullptr;
        DBG("Deleting array with size: %i", m_max_size);
    }
}

template<typename T>
Array<T>::Array(const Array& other)
    : ArrayBase(other), m_store(other.m_store), m_max_size(other.m_max_size),
      m_store_is_internal(false) // prevent double free
{}

template<typename T>
Array<T>::Array(Array&& other) noexcept
    : ArrayBase(std::move(other)), m_store(other.m_store), m_max_size(other.m_max_size),
      m_store_is_internal(other.m_store_is_internal) {
    other.m_store = nullptr;
}

template<typename T>
Array<T>& Array<T>::operator=(const Array& other) {
    if (this == &other) return *this;
    ArrayBase::operator=(other);
    m_store = other.m_store;
    m_max_size = other.m_max_size;
    m_store_is_internal = false; // prevent double free
    return *this;
}

template<typename T>
Array<T>& Array<T>::operator=(Array&& other) noexcept {
    if (this == &other) return *this;
    ArrayBase::operator=(std::move(other));
    m_store = other.m_store;
    other.m_store = nullptr;
    m_max_size = other.m_max_size;
    m_store_is_internal = other.m_store_is_internal;
    return *this;
}

template<typename T>
Array<T>& Array<T>::operator=(const std::initializer_list<T>& value_list) noexcept {
    if (m_store_is_internal) {
        std::free(m_store);
    }
    *this = Array<T>(value_list);
    return *this;
}

// template<typename T>
// template<ArrayBase::Size CAP>
// void Array<T>::setBackingStore(T (&store)[CAP]) {
//     static_assert(CAP > 0, "setting backing store with empty size is not allowed.");
//     m_store = store;
//     m_max_size = CAP;
// }

template<typename T>
T& Array<T>::at(Size index) const {
    return m_store[index];
}

template<typename T>
T& Array<T>::operator[](Size index) {
    return m_store[index];
}

template<typename T>
const T& Array<T>::operator[](Size index) const {
    return m_store[index];
}

template<typename T>
T& Array<T>::front() {
    return (m_store[0]);
}

template<typename T>
T& Array<T>::back() {
    return (m_max_size > 0) ? m_store[m_max_size - 1] : m_store[0];
}

template<typename T>
T* Array<T>::data() {
    return m_store;
}

template<typename T>
const T* Array<T>::data() const {
    return m_store;
}

/* iterators */

template<typename T>
typename Array<T>::iterator Array<T>::begin() {
    return iterator(m_store);
}

template<typename T>
typename Array<T>::iterator Array<T>::end() {
    return iterator(m_store, m_max_size);
}

/* const iterators */

template<typename T>
typename Array<T>::const_iterator Array<T>::begin() const {
    return const_iterator(m_store);
}

template<typename T>
typename Array<T>::const_iterator Array<T>::end() const {
    return const_iterator(m_store, m_max_size);
}

template<typename T>
typename Array<T>::const_iterator Array<T>::cbegin() const {
    return const_iterator(m_store);
}

template<typename T>
typename Array<T>::const_iterator Array<T>::cend() const {
    return const_iterator(m_store, m_max_size);
}

/* reverse iterators */

template<typename T>
typename Array<T>::reverse_iterator Array<T>::rbegin() {
    return reverse_iterator(m_store, m_max_size - 1);
}

template<typename T>
typename Array<T>::reverse_iterator Array<T>::rend() {
    return reverse_iterator(m_store, -1);
}

/* const reverse iterators */

template<typename T>
typename Array<T>::const_reverse_iterator Array<T>::rbegin() const {
    return const_reverse_iterator(m_store, m_max_size - (m_max_size > 0));
}

template<typename T>
typename Array<T>::const_reverse_iterator Array<T>::rend() const {
    return const_reverse_iterator(m_store, (m_max_size - (m_max_size > 0)));
}

template<typename T>
typename Array<T>::const_reverse_iterator Array<T>::crbegin() const {
    return const_reverse_iterator(m_store, m_max_size - (m_max_size > 0));
}

template<typename T>
typename Array<T>::const_reverse_iterator Array<T>::crend() const {
    return const_reverse_iterator(m_store, (m_max_size - (m_max_size > 0)));
}

/* utility */

template<typename T>
bool Array<T>::empty() const {
    return m_max_size == 0;
}

template<typename T>
ArrayBase::Size Array<T>::size() const {
    return m_max_size;
}

template<typename T>
ArrayBase::Size Array<T>::max_size() const {
    return m_max_size;
}

template<typename T>
void Array<T>::swap(Array& other) noexcept {
    T* tmp = this->m_store;
    this->m_store = other.m_store;
    other.m_store = tmp;
}

template<typename T>
void Array<T>::fill(const T& value) {
    for (Size i = 0; i < m_max_size; i++)
        m_store[i] = value;
}

/*******************************************************************************
** Array Iterator Definitions
*******************************************************************************/

template<typename T>
ArrayIterator<T>::ArrayIterator() : m_ptr(nullptr), m_index(0) {}

template<typename T>
ArrayIterator<T>::ArrayIterator(T* store_ptr, ArrayBase::Size index) : m_ptr(store_ptr), m_index(index) {}

template<typename T>
bool ArrayIterator<T>::operator!=(const ArrayIterator<T>& other) const {
    return (other.m_index != this->m_index);
}

template<typename T>
bool ArrayIterator<T>::operator==(const ArrayIterator<T>& other) const {
    return (other.m_index == this->m_index);
}

template<typename T>
ArrayIterator<T>& ArrayIterator<T>::operator++() {
    m_index++;
    return *this;
}

template<typename T>
ArrayIterator<T> ArrayIterator<T>::operator++(int) {
    m_index++;
    return *this;
}

template<typename T>
ArrayIterator<T>& ArrayIterator<T>::operator--() {
    m_index--;
    return *this;
}

template<typename T>
ArrayIterator<T> ArrayIterator<T>::operator--(int) {
    m_index--;
    return *this;
}

template<typename T>
T& ArrayIterator<T>::operator*() const {
    return *(m_ptr + m_index);
}

/* ArrayReverseIterator */

// template<typename T>
// bool ArrayReverseIterator<T>::operator!=(const ArrayReverseIterator<T>& other) const {
//     return this->m_index != other.m_index;
// }
//
// template<typename T>
// bool ArrayReverseIterator<T>::operator==(const ArrayReverseIterator<T>& other) const {
//     return this->m_index == other.m_index;
// }

template<typename T>
bool ArrayReverseIterator<T>::operator!=(const ArrayReverseIterator<T>& other) const {
    return ArrayIterator<T>::operator!=(other);
}

template<typename T>
bool ArrayReverseIterator<T>::operator==(const ArrayReverseIterator<T>& other) const {
    return ArrayIterator<T>::operator==(other);
}

template<typename T>
ArrayReverseIterator<T>& ArrayReverseIterator<T>::operator++() {
    --this->m_index;
    return *this;
}

template<typename T>
ArrayReverseIterator<T> ArrayReverseIterator<T>::operator++(int) {
    --this->m_index;
    return *this;
}
template<typename T>
ArrayReverseIterator<T>& ArrayReverseIterator<T>::operator--() {
    ++this->m_index;
    return *this;
}
template<typename T>
ArrayReverseIterator<T> ArrayReverseIterator<T>::operator--(int) {
    ++this->m_index;
    return *this;
}

} // namespace spn::structure
