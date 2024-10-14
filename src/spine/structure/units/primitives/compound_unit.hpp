#pragma once

#include "spine/core/exception.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

#include <cmath>
#include <cstdint>
#include <numeric>
#include <type_traits>

namespace spn::structure::units {

namespace detail {

template<typename, template<typename...> class>
/// False-type trait for templated base class
struct is_instantiation_of : std::false_type {};

template<typename... Args, template<typename...> class Template>
/// True-type trait for templated base class
struct is_instantiation_of<Template<Args...>, Template> : std::true_type {};

template<typename NumeratorUnit, typename DenominatorUnit, typename OtherNumeratorUnit, typename OtherDenominatorUnit>
/// Returns true if both units have the same unit tags
constexpr bool have_same_unit_tags() {
    return std::is_same_v<
               typename NumeratorUnit::UnitTag,
               typename OtherNumeratorUnit::
                   UnitTag> && std::is_same_v<typename DenominatorUnit::UnitTag, typename OtherDenominatorUnit::UnitTag>;
}

template<typename NumeratorUnit, typename DenominatorUnit, typename OtherNumeratorUnit, typename OtherDenominatorUnit>
/// Returns true if both units have the same unit tags
constexpr bool have_same_value_types() {
    return std::is_same_v<
               typename NumeratorUnit::ValueType,
               typename OtherNumeratorUnit::
                   ValueType> && std::is_same_v<typename DenominatorUnit::ValueType, typename OtherDenominatorUnit::ValueType>;
}

template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
/// Returns true if both units have the same unit tags
constexpr bool are_both_spn_units() {
    return detail::is_instantiation_of<OtherNumeratorUnit, Unit>::value
           && detail::is_instantiation_of<OtherDenominatorUnit, Unit>::value;
}

template<typename NumeratorUnit, typename DenominatorUnit, typename OtherNumeratorUnit, typename OtherDenominatorUnit>
/// Returns true if Numerator/Denominator and their respective others are interchangeable
constexpr bool are_interchangeable() {
    return are_both_spn_units<OtherNumeratorUnit, OtherDenominatorUnit>()
           && have_same_value_types<NumeratorUnit, DenominatorUnit, OtherNumeratorUnit, OtherDenominatorUnit>()
           && have_same_unit_tags<NumeratorUnit, DenominatorUnit, OtherNumeratorUnit, OtherDenominatorUnit>();
}

} // namespace detail

template<typename NumeratorUnit, typename DenominatorUnit>
class CompoundUnit {
public:
    using NumeratorType = NumeratorUnit;
    using DenominatorType = DenominatorUnit;
    using ValueType = typename NumeratorUnit::ValueType;

    static_assert(detail::is_instantiation_of<NumeratorUnit, Unit>::value, "Numerator must be of type `Unit`");
    static_assert(detail::is_instantiation_of<DenominatorUnit, Unit>::value, "Denominator must be of type `Unit`");
    static_assert(std::is_same_v<typename NumeratorUnit::ValueType, typename DenominatorUnit::ValueType>,
                  "Numerator and denominator must have same value type");

    constexpr CompoundUnit() = default;
    CompoundUnit(const NumeratorUnit& numerator, const DenominatorUnit& denominator)
        : _numerator(numerator), _denominator(denominator) {}
    CompoundUnit(const CompoundUnit&) = default;
    CompoundUnit& operator=(const CompoundUnit&) = default;
    CompoundUnit(CompoundUnit&& other) noexcept = default;
    CompoundUnit& operator=(CompoundUnit&& other) noexcept = default;

    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit,
             std::enable_if_t<detail::are_interchangeable<NumeratorUnit, DenominatorUnit, OtherNumeratorUnit,
                                                          OtherDenominatorUnit>(),
                              int> = 0>
    CompoundUnit(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other)
        : _numerator(other.numerator()), _denominator(other.denominator()) {}

    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit,
             std::enable_if_t<detail::are_interchangeable<NumeratorUnit, DenominatorUnit, OtherNumeratorUnit,
                                                          OtherDenominatorUnit>(),
                              int> = 0>
    CompoundUnit& operator=(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) {
        _numerator = NumeratorUnit(other.numerator());
        _denominator = DenominatorUnit(other.denominator());
        return *this;
    }

    const NumeratorUnit& numerator() const { return _numerator; }
    const DenominatorUnit& denominator() const { return _denominator; }

    [[nodiscard]] constexpr ValueType raw() const {
        if (_denominator.raw() == 0) spn::throw_exception(runtime_exception("tried to divide by zero"));
        return _numerator.raw() / _denominator.raw();
    }

    /// Simplify compound by reducing numerator and denominator by greatest common denominator. Provided
    /// `scaling_factor` determines precision. A `scaling_factor` of 1e6 makes 6 decimals of precision.
    void simplify(RealRawType scaling_factor = 1e6) {
        const auto gcd = compute_gcd(_numerator.raw(), _denominator.raw(), scaling_factor);
        _numerator /= gcd;
        _denominator /= gcd;
    }

    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit,
             std::enable_if_t<detail::are_interchangeable<NumeratorUnit, DenominatorUnit, OtherNumeratorUnit,
                                                          OtherDenominatorUnit>(),
                              int> = 0>
    CompoundUnit& operator+=(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) {
        static_assert(std::is_constructible_v<NumeratorUnit, OtherNumeratorUnit>,
                      "NumeratorUnit must be constructible from OtherNumeratorUnit");
        static_assert(std::is_constructible_v<DenominatorUnit, OtherDenominatorUnit>,
                      "DenominatorUnit must be constructible from OtherDenominatorUnit");

        // convert to common units
        const auto converted_numerator = NumeratorUnit(other.numerator());
        const auto converted_denominator = DenominatorUnit(other.denominator());

        _numerator = NumeratorUnit(_numerator.raw() * converted_denominator.raw()
                                   + converted_numerator.raw() * _denominator.raw());
        _denominator = DenominatorUnit(_denominator.raw() * converted_denominator.raw());
        return *this;
    }

    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    auto operator+(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        CompoundUnit result = *this;
        result += other;
        return result;
    }

    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit,
             std::enable_if_t<detail::are_interchangeable<NumeratorUnit, DenominatorUnit, OtherNumeratorUnit,
                                                          OtherDenominatorUnit>(),
                              int> = 0>
    CompoundUnit& operator-=(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) {
        static_assert(std::is_constructible_v<NumeratorUnit, OtherNumeratorUnit>,
                      "NumeratorUnit must be constructible from OtherNumeratorUnit");
        static_assert(std::is_constructible_v<DenominatorUnit, OtherDenominatorUnit>,
                      "DenominatorUnit must be constructible from OtherDenominatorUnit");

        // convert to common units
        const auto converted_numerator = NumeratorUnit(other.numerator());
        const auto converted_denominator = DenominatorUnit(other.denominator());

        _numerator = _numerator * converted_denominator.raw() - converted_numerator * _denominator.raw();
        _denominator = _denominator * converted_denominator.raw();
        return *this;
    }

    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    auto operator-(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        CompoundUnit result = *this;
        result -= other;
        return result;
    }
    [[nodiscard]] CompoundUnit operator-() const { return CompoundUnit(numerator(), -denominator()); }

    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, CompoundUnit> operator*(const AT& scalar) const {
        return CompoundUnit{_numerator * scalar, _denominator};
    }
    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, CompoundUnit&> operator*=(const AT& scalar) {
        _numerator *= scalar;
        return *this;
    }
    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, CompoundUnit> operator/(const AT& scalar) const {
        if (scalar == 0) spn::throw_exception(runtime_exception("tried to divide by zero"));
        return CompoundUnit{_numerator / scalar, _denominator};
    }
    template<typename AT>
    std::enable_if_t<std::is_arithmetic_v<AT>, CompoundUnit&> operator/=(const AT& scalar) {
        if (scalar == 0) spn::throw_exception(runtime_exception("tried to divide by zero"));
        _numerator /= scalar;
        return *this;
    }

    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    bool operator==(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        if constexpr (std::is_integral_v<ValueType>) return raw() == other.raw();
        return approximately_equal(raw(), other.raw());
    }
    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    bool operator!=(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        return !(*this == other);
    }
    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    bool operator<(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        return raw() < other.raw();
    }
    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    bool operator>(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        return raw() > other.raw();
    }
    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    bool operator<=(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        return raw() <= other.raw();
    }
    template<typename OtherNumeratorUnit, typename OtherDenominatorUnit>
    bool operator>=(const CompoundUnit<OtherNumeratorUnit, OtherDenominatorUnit>& other) const {
        return raw() >= other.raw();
    }

private:
    static constexpr ValueType epsilon = std::numeric_limits<ValueType>::epsilon();

    bool approximately_equal(ValueType a, ValueType b) const {
        return std::fabs(a - b) <= epsilon * std::max(std::fabs(a), std::fabs(b));
    }

    inline unsigned long scale_to_integer(ValueType num, RealRawType scaling_factor) {
        return static_cast<unsigned long>(std::round(num * scaling_factor));
    }
    unsigned long compute_gcd(ValueType numerator, ValueType denominator, RealRawType scaling_factor) {
        if constexpr (std::is_integral_v<ValueType>) return std::gcd(numerator, denominator);
        return std::gcd(scale_to_integer(numerator, scaling_factor), scale_to_integer(denominator, scaling_factor));
    }

private:
    NumeratorUnit _numerator;
    DenominatorUnit _denominator;
};

} // namespace spn::structure::units