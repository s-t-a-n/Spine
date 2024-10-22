#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <cstdint>
#include <utility>

namespace spn::filter {

template<typename ValueType = float>
/// Filter that does nothing but pas through values. Meant for stubbing applications
class Passthrough : public Filter<ValueType> {
public:
    struct Config {};

    Passthrough(const Config&& cfg) : _cfg(std::move(cfg)) {
        static_assert(std::is_floating_point_v<ValueType>, "ValueType must be a floating point type");
    }
    ~Passthrough() override = default;

    /// Take a new sample, returns filtered value
    ValueType value(const ValueType sample) override {
        new_sample(sample);
        return _value;
    }

    /// Returns filtered value
    ValueType value() const override { return _value; }
    ValueType operator()(ValueType sample) { return value(sample); }

    /// Takes a new sample (syntax sugar)
    void new_sample(const ValueType sample) override { _value = sample; }

    /// Reset the filter's internal value to the provided value (has no effect on this filter)
    void reset_to(ValueType value) override { _value = value; }

private:
    const Config _cfg;
    ValueType _value = 0;
};

} // namespace spn::filter
