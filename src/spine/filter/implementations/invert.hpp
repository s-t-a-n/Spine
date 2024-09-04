#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <cstdint>
#include <utility>

namespace spn::filter {

// Map a value from an input range to and output range
template<typename ValueType = double>
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

    static std::unique_ptr<Invert> Invertor(double input_lower_limit = 0.0, double input_upper_limit = 1.0) {
        return std::make_unique<Invert>(Config{
            .input_lower_limit = input_lower_limit,
            .input_upper_limit = input_upper_limit,
        });
    }

    ValueType value(const ValueType sample) override {
        assert(sample >= _cfg.input_lower_limit && sample <= _cfg.input_upper_limit);
        return _value = _cfg.input_upper_limit - (sample - _cfg.input_lower_limit);
    }

    ValueType value() const override { return _value; }

    void new_sample(ValueType sample) override { value(sample); }

    void reset_to(ValueType value) override { _value = value; }

    ValueType operator()(ValueType reading) { return value(reading); }

private:
    const Config _cfg;
    ValueType _value = 0;
};

} // namespace spn::filter
