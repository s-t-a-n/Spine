#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <cstdint>
#include <utility>

namespace spn::filter {

// Map a value from an input range to and output range
template<typename ValueType = double>
class MappedRange : public Filter<ValueType> {
public:
    struct Config {
        ValueType input_lower_limit = 0;
        ValueType input_upper_limit = 1.0;
        ValueType output_lower_limit = 0;
        ValueType output_upper_limit = 100;

        bool throw_for_value_out_of_range = false;
    };

    MappedRange(const Config&& cfg) : _cfg(std::move(cfg)) {
        static_assert(std::is_floating_point<ValueType>::value, "ValueType must be a floating point type");
    }
    ~MappedRange() override = default;

    static std::unique_ptr<MappedRange> Percentage(double input_lower_limit = 0.0, double input_upper_limit = 1.0) {
        return std::make_unique<MappedRange>(Config{.input_lower_limit = input_lower_limit,
                                                    .input_upper_limit = input_upper_limit,
                                                    .output_lower_limit = 0,
                                                    .output_upper_limit = 100});
    }
    static std::unique_ptr<MappedRange> Permillage(double input_lower_limit = 0.0, double input_upper_limit = 1.0) {
        return std::make_unique<MappedRange>(Config{.input_lower_limit = input_lower_limit,
                                                    .input_upper_limit = input_upper_limit,
                                                    .output_lower_limit = 0,
                                                    .output_upper_limit = 1000});
    }

    ValueType value(ValueType sample) override {
        if (sample < _cfg.input_lower_limit || sample > _cfg.input_upper_limit) {
            DBG("--------------------------------------------------------------------------");
            DBG("MappedRange rejected value of %f, limits: lower: %f, upper: %f", sample, _cfg.input_lower_limit,
                _cfg.input_upper_limit);
            DBG("--------------------------------------------------------------------------");
            if (_cfg.throw_for_value_out_of_range) {
                spn::throw_exception(spn::runtime_exception("Value out of range"));
            }
            sample = std::clamp(sample, _cfg.input_lower_limit, _cfg.input_upper_limit);
        }
        return _value = _cfg.output_lower_limit
                        + (sample - _cfg.input_lower_limit) / (_cfg.input_upper_limit - _cfg.input_lower_limit)
                              * (_cfg.output_upper_limit - _cfg.output_lower_limit);
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
