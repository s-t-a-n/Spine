#pragma once

#include <cmath>
#include <cstdint>
#include <ctime>

using TimeRaw = time_t;

namespace spn::core::time::detail {

template<typename Tag>
class Time {
public:
    Time() = default;
    explicit Time(const TimeRaw length) : _value(length) {}

    template<typename ValueType = TimeRaw>
    ValueType raw() const {
        return static_cast<ValueType>(_value);
    }

    bool is_negative() const { return _value < 0; }
    uint32_t printable() const { return static_cast<uint32_t>(std::abs(_value)); }

    template<typename TagOther>
    Time(const Time<TagOther>& other) : _value(from_other(other)) {}

    template<typename TagOther>
    Time<Tag>& operator=(const Time<TagOther>& other) {
        _value = from_other(other);
        return *this;
    }

    template<typename TagOther>
    Time<Tag>& operator=(Time<TagOther>&& other) {
        _value = from_other(other);
        return *this;
    }

    template<typename TagOther>
    Time<Tag>& operator-=(const Time<TagOther>& other) {
        _value -= from_other(other);
        return *this;
    }

    template<typename TagOther>
    Time<Tag>& operator+=(const Time<TagOther>& other) {
        _value += from_other(other);
        return *this;
    }

    template<typename TagOther>
    Time<Tag> operator*(const Time<TagOther>& other) const {
        return Time<Tag>{_value * from_other(other)};
    }

    // template<typename ArithmeticType>
    // Time<Tag> operator*(const ArithmeticType multiplier) const {
    //     static_assert(std::is_arithmetic_v<ArithmeticType>, "Multiplier must be arithmetic");
    //     return Time<Tag>{_value * multiplier};
    // }

    template<typename TagOther>
    Time<Tag> operator+(const Time<TagOther>& other) const {
        return Time<Tag>{_value + from_other(other)};
    }

    template<typename TagOther>
    Time<Tag> operator-(const Time<TagOther>& other) const {
        return Time<Tag>{_value - from_other(other)};
    }

    template<typename TagOther>
    Time<Tag> operator/(const Time<TagOther>& other) const {
        return Time<Tag>{_value / from_other(other)};
    }

    // template<typename ArithmeticType>
    // Time<Tag> operator/(const ArithmeticType multiplier) const {
    //     static_assert(std::is_arithmetic_v<ArithmeticType>, "Divider must be arithmetic");
    //     return Time<Tag>{_value / multiplier};
    // }

    template<typename TagOther>
    bool operator==(const Time<TagOther>& other) const {
        return _value == from_other(other);
    }

    template<typename TagOther>
    bool operator!=(const Time<TagOther>& other) const {
        return _value != from_other(other);
    }

    template<typename TagOther>
    bool operator<(const Time<TagOther>& other) const {
        return _value < from_other(other);
    }

    template<typename TagOther>
    bool operator>(const Time<TagOther>& other) const {
        return _value > from_other(other);
    }

    template<typename TagOther>
    bool operator<=(const Time<TagOther>& other) const {
        return _value <= from_other(other);
    }

    template<typename TagOther>
    bool operator>=(const Time<TagOther>& other) const {
        return _value >= from_other(other);
    }

private:
    TimeRaw _value;

    template<typename TagOther>
    TimeRaw from_other(const Time<TagOther>& other) const {
        return static_cast<TimeRaw>(TagOther::Magnitude / Tag::Magnitude * other.raw());
    }
};

template<typename Tag, typename ArithmeticType>
std::enable_if_t<std::is_arithmetic_v<ArithmeticType> && !std::is_same_v<ArithmeticType, Time<Tag>>, Time<Tag>>
operator*(const Time<Tag>& value, const ArithmeticType multiplier) {
    return Time<Tag>{value.raw() * multiplier};
}

template<typename Tag, typename ArithmeticType>
std::enable_if_t<std::is_arithmetic_v<ArithmeticType> && !std::is_same_v<ArithmeticType, Time<Tag>>, Time<Tag>>
operator*(const ArithmeticType multiplier, const Time<Tag>& value) {
    return Time<Tag>{multiplier * value.raw()};
}

template<typename Tag, typename ArithmeticType>
std::enable_if_t<std::is_arithmetic_v<ArithmeticType> && !std::is_same_v<ArithmeticType, Time<Tag>>, Time<Tag>>
operator/(const Time<Tag>& value, const ArithmeticType multiplier) {
    return Time<Tag>{value.raw() / multiplier};
}

template<typename Tag, typename ArithmeticType>
std::enable_if_t<std::is_arithmetic_v<ArithmeticType> && !std::is_same_v<ArithmeticType, Time<Tag>>, Time<Tag>>
operator/(const ArithmeticType multiplier, const Time<Tag>& value) {
    return Time<Tag>{multiplier / value.raw()};
}

struct MicrosecondsTag : public Time<MicrosecondsTag> {
    using Time<MicrosecondsTag>::Time;
    static constexpr double Magnitude = 1e-6;
};
struct MillisecondsTag : public Time<MillisecondsTag> {
    using Time<MillisecondsTag>::Time;
    static constexpr double Magnitude = 1e-3;
};
struct SecondsTag : public Time<SecondsTag> {
    using Time<SecondsTag>::Time;
    static constexpr double Magnitude = 1.0;
};
struct MinutesTag : public Time<MinutesTag> {
    using Time<MinutesTag>::Time;
    static constexpr double Magnitude = 60.0;
};
struct HoursTag : public Time<HoursTag> {
    using Time<HoursTag>::Time;
    static constexpr double Magnitude = 3600.0; // 60*60
};
struct DaysTag : public Time<DaysTag> {
    using Time<DaysTag>::Time;
    static constexpr double Magnitude = 86400.0; // 60*60*24
};

} // namespace spn::core::time::detail

using time_us = spn::core::time::detail::Time<spn::core::time::detail::MicrosecondsTag>;
using time_ms = spn::core::time::detail::Time<spn::core::time::detail::MillisecondsTag>;
using time_s = spn::core::time::detail::Time<spn::core::time::detail::SecondsTag>;
using time_m = spn::core::time::detail::Time<spn::core::time::detail::MinutesTag>;
using time_h = spn::core::time::detail::Time<spn::core::time::detail::HoursTag>;
using time_d = spn::core::time::detail::Time<spn::core::time::detail::DaysTag>;
