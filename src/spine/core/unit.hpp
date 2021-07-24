#pragma once

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace spn::core {

/// Abstract unit with UnitTag U, Magnitude tag M, and ValueType T
template<typename U, typename M, typename T>
class Unit {
public:
    Unit() = default;
    explicit Unit(const T length) : _value(length) {}

    template<typename ValueType = T>
    ValueType raw() const {
        return static_cast<ValueType>(_value);
    }

    bool is_negative() const { return _value < 0; }
    uint32_t printable() const { return static_cast<uint32_t>(std::abs(_value)); }

    template<typename TagOther>
    Unit(const Unit<U, TagOther, T>& other) : _value(from_other(other)) {}

    template<typename MOther>
    Unit<U, M, T>& operator=(const Unit<U, MOther, T>& other) {
        _value = from_other(other);
        return *this;
    }

    template<typename MOther>
    Unit<U, M, T>& operator=(Unit<U, MOther, T>&& other) {
        _value = from_other(other);
        return *this;
    }

    template<typename MOther>
    Unit<U, M, T>& operator-=(const Unit<U, MOther, T>& other) {
        _value -= from_other(other);
        return *this;
    }

    template<typename MOther>
    Unit<U, M, T>& operator+=(const Unit<U, MOther, T>& other) {
        _value += from_other(other);
        return *this;
    }

    template<typename MOther>
    Unit<U, M, T> operator*(const Unit<U, MOther, T>& other) const {
        return Unit<U, M, T>{_value * from_other(other)};
    }

    // template<typename ArithmeticType>
    // Unit<Tag> operator*(const ArithmeticType multiplier) const {
    //     static_assert(std::is_arithmetic_v<ArithmeticType>, "Multiplier must be arithmetic");
    //     return Unit<Tag>{_value * multiplier};
    // }

    template<typename TagOther>
    Unit<U, M, T> operator+(const Unit<U, TagOther, T>& other) const {
        return Unit<U, M, T>{_value + from_other(other)};
    }

    template<typename TagOther>
    Unit<U, M, T> operator-(const Unit<U, TagOther, T>& other) const {
        return Unit<U, M, T>{_value - from_other(other)};
    }

    template<typename TagOther>
    Unit<U, M, T> operator/(const Unit<U, TagOther, T>& other) const {
        return Unit<U, M, T>{_value / from_other(other)};
    }

    // template<typename ArithmeticType>
    // Unit<Tag> operator/(const ArithmeticType multiplier) const {
    //     static_assert(std::is_arithmetic_v<ArithmeticType>, "Divider must be arithmetic");
    //     return Unit<Tag>{_value / multiplier};
    // }

    template<typename TagOther>
    bool operator==(const Unit<U, TagOther, T>& other) const {
        return _value == from_other(other);
    }

    template<typename TagOther>
    bool operator!=(const Unit<U, TagOther, T>& other) const {
        return _value != from_other(other);
    }

    template<typename TagOther>
    bool operator<(const Unit<U, TagOther, T>& other) const {
        return _value < from_other(other);
    }

    template<typename TagOther>
    bool operator>(const Unit<U, TagOther, T>& other) const {
        return _value > from_other(other);
    }

    template<typename TagOther>
    bool operator<=(const Unit<U, TagOther, T>& other) const {
        return _value <= from_other(other);
    }

    template<typename TagOther>
    bool operator>=(const Unit<U, TagOther, T>& other) const {
        return _value >= from_other(other);
    }

private:
    T _value = 0;

    template<typename TagOther>
    T from_other(const Unit<U, TagOther, T>& other) const {
        return static_cast<T>(TagOther::Magnitude / M::Magnitude * other.raw());
    }
};

template<typename U, typename M, typename T, typename AT>
std::enable_if_t<std::is_arithmetic_v<AT> && !std::is_same_v<AT, Unit<U, M, T>>, Unit<U, M, T>>
operator*(const Unit<U, M, T>& value, const AT multiplier) {
    return Unit<U, M, T>{value.raw() * multiplier};
}

template<typename U, typename M, typename T, typename AT>
std::enable_if_t<std::is_arithmetic_v<AT> && !std::is_same_v<AT, Unit<U, M, T>>, Unit<U, M, T>>
operator*(const AT multiplier, const Unit<U, M, T>& value) {
    return Unit<U, M, T>{multiplier * value.raw()};
}

template<typename U, typename M, typename T, typename AT>
std::enable_if_t<std::is_arithmetic_v<AT> && !std::is_same_v<AT, Unit<U, M, T>>, Unit<U, M, T>>
operator/(const Unit<U, M, T>& value, const AT multiplier) {
    return Unit<U, M, T>{value.raw() / multiplier};
}

template<typename U, typename M, typename T, typename AT>
std::enable_if_t<std::is_arithmetic_v<AT> && !std::is_same_v<AT, Unit<U, M, T>>, Unit<U, M, T>>
operator/(const AT multiplier, const Unit<U, M, T>& value) {
    return Unit<U, M, T>{multiplier / value.raw()};
}

} // namespace spn::core