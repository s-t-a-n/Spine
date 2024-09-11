#pragma once

#include "spine/structure/vector.hpp"

namespace spn::structure {

template<typename T>
/// A Vector that only allows Last-In-First-Out access order.
class Stack : public Vector<T> {
public:
    using Size = ArrayBase::Size;

    Stack(Size size) : store(size) {}

    /// Push an element unto the stack.
    void push(T& element) { emplace(std::forward<T>(element)); }

    /// Pop an element from the stack.
    void pop() { store.pop_back(); }

    /// Returns a reference to the element on top without removing it (dangerous!).
    T& top() {
        T& element = store.peek_back();
        return element;
    }

    /// Push an element unto the stack by emplacing it.
    void emplace(T&& element) { store.emplace_back(std::move(element)); };

    /// Returns size of stack.
    Size size() { return store.size(); }

    /// Returns true if stack is empty.
    bool empty() { return store.empty(); }

private:
    Vector<T> store;
};

} // namespace spn::structure
