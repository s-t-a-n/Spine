#pragma once

template<typename T>
constexpr int ENUM_IDX(T idx) {
    return static_cast<int>(idx);
}
