#pragma once

// #define ENUM_IDX(__ENUM_IDX) static_cast<int>(__ENUM_IDX)

template<typename T>
constexpr int ENUM_IDX(T idx) {
    return static_cast<int>(idx);
}
