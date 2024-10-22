#pragma once

#include "spine/core/exception.hpp"

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace spn::structure::units {

/// Abstract unit with UnitTag U, Magnitude tag M, and ValueType T
template<typename UT, typename MT, typename VT>
class Unit {
public:
    using UnitTag = UT;
    using ValueType = VT;

    static_assert(std::is_arithmetic_v<VT>, "ValueType T must be an arithmetic type.");

    constexpr Unit() = default;
    constexpr explicit Unit(const VT length) : _value(length) {}

    Unit(const Unit&) = default;
    Unit& operator=(const Unit&) = default;
    Unit(Unit&&) = default;
    Unit& operator=(Unit&&) = default;

    template<typename T = ValueType>
    constexpr T raw() const {
        return static_cast<T>(_value);
    }

    bool is_negative() const { return _value < 0; }
    static constexpr bool is_integral() { return std::is_integral_v<VT>; }

    template<typename MOther>
    Unit(const Unit<UT, MOther, VT>& other) : _value(from_other(other)) {}

    template<typename MOther>
    Unit<UT, MT, VT>& operator=(const Unit<UT, MOther, VT>& other) {
        _value = from_other(other);
        return *this;
    }
    template<typename MOther>
    Unit<UT, MT, VT>& operator=(Unit<UT, MOther, VT>&& other) {
        _value = from_other(other);
        return *this;
    }

    template<typename MOther>
    [[nodiscard]] Unit<UT, MT, VT> operator+(const Unit<UT, MOther, VT>& other) const {
        return Unit<UT, MT, VT>{_value + from_other(other)};
    }
    template<typename MOther>
    [[nodiscard]] Unit<UT, MT, VT> operator-(const Unit<UT, MOther, VT>& other) const {
        return Unit<UT, MT, VT>{_value - from_other(other)};
    }
    [[nodiscard]] Unit operator-() const { return Unit(-_value); }
    template<typename MOther>
    Unit<UT, MT, VT>& operator-=(const Unit<UT, MOther, VT>& other) {
        _value -= from_other(other);
        return *this;
    }
    template<typename MOther>
    Unit<UT, MT, VT>& operator+=(const Unit<UT, MOther, VT>& other) {
        _value += from_other(other);
        return *this;
    }
    template<typename MOther>
    /// Unstable API: todo: this should return an area
    [[nodiscard]] Unit<UT, MT, VT> operator*(const Unit<UT, MOther, VT>& other) const {
        return Unit<UT, MT, VT>{_value * from_other(other)};
    }
    template<typename MOther>
    [[nodiscard]] Unit<UT, MT, VT> operator/(const Unit<UT, MOther, VT>& other) const {
        const auto other_value = from_other(other);
        if (other_value == 0) spn::throw_exception(runtime_exception("tried to divide by zero"));
        return Unit<UT, MT, VT>{_value / other_value};
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
    bool operator==(const Unit<UT, MOther, VT>& other) const {
        if constexpr (std::is_integral_v<VT>) return _value == from_other(other);
        return approximately_equal(_value, from_other(other));
    }
    template<typename MOther>
    bool operator!=(const Unit<UT, MOther, VT>& other) const {
        return !(*this == other);
    }
    template<typename MOther>
    bool operator<(const Unit<UT, MOther, VT>& other) const {
        return _value < from_other(other);
    }
    template<typename MOther>
    bool operator>(const Unit<UT, MOther, VT>& other) const {
        return _value > from_other(other);
    }
    template<typename MOther>
    bool operator<=(const Unit<UT, MOther, VT>& other) const {
        return _value <= from_other(other);
    }
    template<typename MOther>
    bool operator>=(const Unit<UT, MOther, VT>& other) const {
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
    ValueType from_other(const Unit<UT, MOther, VT>& other) const {
        constexpr float ratio = MOther::Magnitude / MT::Magnitude;
        if constexpr (is_integral()) return std::round(ratio * other.raw());
        return ratio * other.raw();
    }
};

} // namespace spn::structure::units

namespace spnu = spn::structure::units;