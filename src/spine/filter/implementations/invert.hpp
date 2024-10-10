#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <cstdint>
#include <utility>

namespace spn::filter {

template<typename ValueType = double>
/// Filter that takes a value and inverts it
class Invert : public Filter<ValueType> {
public:
    struct Config {
        ValueType input_lower_limit = 0.0;
        ValueType input_upper_limit = 1.0;
    };

    Invert(const Config&& cfg) : _cfg(std::move(cfg)) {
        static_assert(std::is_floating_point<ValueType>::value, "ValueType must be a floating point type");
    }
    ~Invert() override = default;

    /// Provides a normalized inverter
    static std::unique_ptr<Invert> NormalizedInverter(double input_lower_limit = 0.0, double input_upper_limit = 1.0) {
        return std::make_unique<Invert>(Config{
            .input_lower_limit = input_lower_limit,
            .input_upper_limit = input_upper_limit,
        });
    }

    /// Take a new sample, returns filtered value
    ValueType value(const ValueType sample) override {
        spn_assert(sample >= _cfg.input_lower_limit && sample <= _cfg.input_upper_limit);
        return _value = _cfg.input_upper_limit - (sample - _cfg.input_lower_limit);
    }

    /// Returns filtered value
    ValueType value() const override { return _value; }
    ValueType operator()(ValueType reading) { return value(reading); }

    /// Takes a new sample (syntax sugar)
    void new_sample(ValueType sample) override { value(sample); }

    /// Reset the filter's internal value to the provided value (has no effect on this filter)
    void reset_to(ValueType value) override { _value = value; }

private:
    const Config _cfg;
    ValueType _value = 0;
};

} // namespace spn::filter
