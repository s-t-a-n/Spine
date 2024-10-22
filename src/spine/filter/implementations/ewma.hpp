#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <cstdint>
#include <utility>

namespace spn::filter {

template<typename ValueType = float>
/// Exponential weighted moving average
class EWMA : public Filter<ValueType> {
public:
    struct Config {
        ValueType K; // length of frame
    };

    EWMA(const Config&& cfg) : _cfg(std::move(cfg)) {
        static_assert(std::is_floating_point<ValueType>::value, "ValueType must be a floating point type");
    }
    ~EWMA() override = default;

    static std::unique_ptr<EWMA> Short() { return std::make_unique<EWMA>(Config{.K = 10}); }
    static std::unique_ptr<EWMA> Long() { return std::make_unique<EWMA>(Config{.K = 100}); }

    /// Takes a new sample, returns filtered value
    ValueType value(const ValueType sample) override {
        if (_n == 0) {
            ++_n;
            return _value = sample;
        }
        return _value = _value + ((sample - _value) / std::min(++_n, _cfg.K));
    }

    /// Returns filtered value
    ValueType value() const override { return _value; }
    ValueType operator()(ValueType reading) { return value(reading); }

    /// Takes a new sample (syntax sugar)
    void new_sample(ValueType sample) override { value(sample); }

    /// Reset the filter's internal value to the provided value, discarding all earlier values
    void reset_to(ValueType value) override {
        _value = value;
        _n = 0;
    }

private:
    const Config _cfg;

    ValueType _n = 0;
    ValueType _value = 0;
};

} // namespace spn::filter
