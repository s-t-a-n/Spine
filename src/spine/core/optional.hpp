#pragma once

// template<typename T>
// class Optional {
// public:
//     Optional(nullptr_t ptr) : Optional(T & obj) : _obj(obj) {}

//     constexpr const T* operator->() const { return _obj; }
//     constexpr T* operator->() const { return _obj; }
//     constexpr const T& operator*() const& { return _obj; }
//     constexpr T& operator*() & { return _obj; }
//     constexpr const T&& operator*() const&& { return _obj; }
//     constexpr T&& operator*() && { return _obj; }

// private:
//     T& _obj;
// };