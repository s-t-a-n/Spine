#pragma once

#include "spine/core/exception.hpp"

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace spn::structure::units {

/// Abstract unit with UnitTag U, Magnitude tag M, and ValueType T
template<typename U, typename M, typename T>
class Unit {
public:
    using UnitTag = U;
    using ValueType = T;

    static_assert(std::is_arithmetic_v<T>, "ValueType T must be an arithmetic type.");

    constexpr Unit() = default;
    constexpr explicit Unit(const T length) : _value(length) {}

    Unit(const Unit&) = default;
    Unit& operator=(const Unit&) = default;
    Unit(Unit&&) = default;
    Unit& operator=(Unit&&) = default;

    constexpr ValueType raw() const { return _value; }

    bool is_negative() const { return _value < 0; }
    bool is_integral() const {
        if constexpr (std::is_integral_v<T>) return true;
        return std::floor(_value) == _value;
    }

    template<typename MOther>
    Unit(const Unit<U, MOther, T>& other) : _value(from_other(other)) {}

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
    [[nodiscard]] Unit<U, M, T> operator+(const Unit<U, MOther, T>& other) const {
        return Unit<U, M, T>{_value + from_other(other)};
    }
    template<typename MOther>
    [[nodiscard]] Unit<U, M, T> operator-(const Unit<U, MOther, T>& other) const {
        return Unit<U, M, T>{_value - from_other(other)};
    }
    [[nodiscard]] Unit operator-() const { return Unit(-_value); }
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

    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, Unit> operator*(const AT& scalar) const {
        return Unit{_value * scalar};
    }
    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, Unit&> operator*=(const AT& scalar) {
        _value *= scalar;
        return *this;
    }
    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, Unit> operator/(const AT& scalar) const {
        if (scalar == 0) spn::throw_exception(runtime_exception("tried to divide by zero"));
        return Unit{_value / scalar};
    }
    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, Unit&> operator/=(const AT& scalar) {
        if (scalar == 0) spn::throw_exception(runtime_exception("tried to divide by zero"));
        _value /= scalar;
        return *this;
    }

    template<typename MOther>
    bool operator==(const Unit<U, MOther, T>& other) const {
        if constexpr (std::is_integral_v<T>) return _value == from_other(other);
        return approximately_equal(_value, from_other(other));
    }
    template<typename MOther>
    bool operator!=(const Unit<U, MOther, T>& other) const {
        return !(*this == other);
    }
    template<typename MOther>
    bool operator<(const Unit<U, MOther, T>& other) const {
        return _value < from_other(other);
    }
    template<typename MOther>
    bool operator>(const Unit<U, MOther, T>& other) const {
        return _value > from_other(other);
    }
    template<typename MOther>
    bool operator<=(const Unit<U, MOther, T>& other) const {
        return _value <= from_other(other);
    }
    template<typename MOther>
    bool operator>=(const Unit<U, MOther, T>& other) const {
        return _value >= from_other(other);
    }

private:
    static constexpr ValueType epsilon = std::numeric_limits<ValueType>::epsilon();

    bool approximately_equal(ValueType a, ValueType b) const {
        return std::fabs(a - b) <= epsilon * std::max(std::fabs(a), std::fabs(b));
    }

private:
    ValueType _value = 0;

    template<typename MOther>
    ValueType from_other(const Unit<U, MOther, T>& other) const {
        constexpr auto ratio = MOther::Magnitude / M::Magnitude;
        return ratio * other.raw();
    }
};

} // namespace spn::structure::units

namespace spnu = spn::structure::units;