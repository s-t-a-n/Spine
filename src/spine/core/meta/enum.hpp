#pragma once

namespace spn::core::meta {

template<typename T>
constexpr int ENUM_IDX(T idx) {
    return static_cast<int>(idx);
}

} // namespace spn::core::meta