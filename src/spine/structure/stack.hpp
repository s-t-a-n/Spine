#pragma once

#include "spine/structure/vector.hpp"

namespace spn::structure {

template<typename T>
class Stack : public Vector<T> {
public:
    using Size = ArrayBase::Size;

public:
    Stack(Size size) : store(size) {}

    void push(T& element) { store.push_back(element); }

    void pop() { store.pop_back(); }

    T& top() {
        T& element = store.peek_back();
        return element;
    }

    void emplace(T&& element) { store.emplace_back(element); };

    Size size() { return store.size(); }

    bool empty() { return store.empty(); }

private:
    spn::structure::Vector<T> store;
};

} // namespace spn::structure
