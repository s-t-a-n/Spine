#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <spine/platform/hal.hpp>

#include <cmath>
#include <cstdint>
#include <utility>

namespace spn::filter {

// Accepts values when they fall within limits
template<typename ValueType = double>
class BandPass : public Filter<ValueType> {
public:
    enum class Mode { RELATIVE, ABSOLUTE };

    struct Config {
        // RELATIVE applies bandpass relatively @ offset + value
        // ABSOLUTE applies bandpass in the absolute @ offset
        Mode mode = BandPass::Mode::RELATIVE;

        // logarithmic ratio to offset, around offset =>
        // upper=offset+mantissa*10^(decades+1),
        // lower=offset-mantissa*10^(decades+1)
        ValueType mantissa = 1.0;
        ValueType decades = 0.01;

        //  for RELATIVE => for a given N the bandpass envelops the given value + N
        //  for ABSOLUTE => for a given N the bandpass envelops N
        ValueType offset = 0;

        // for any `rejection_limit` beyond 0, a new value will be accepted when `rejection_limit` was reached
        int rejection_limit = 10;
        // when `throw_on_rejection_limit` is true, an exception will be thrown when `rejection_limit` was reached
        bool throw_on_rejection_limit = true;
    };

    static std::unique_ptr<BandPass> Broad() {
        return std::make_unique<BandPass>(Config{.mode = Mode::RELATIVE,
                                                 .mantissa = 1,
                                                 .decades = 0.01,
                                                 .offset = 0,
                                                 .rejection_limit = 10,
                                                 .throw_on_rejection_limit = true});
    }

    // K: length of frame
    BandPass(const Config&& cfg) : _cfg(std::move(cfg)) {
        static_assert(std::is_floating_point_v<ValueType>, "ValueType must be a floating point type");
        assert(_cfg.mantissa >= 1.0 && _cfg.mantissa < 10.0); // sanity
        BandPass::update_limits();
    }
    ~BandPass() override = default;

    ValueType value(const ValueType sample) override {
        new_sample(sample);
        return _value;
    }

    void new_sample(const ValueType sample) override {
        if (_value == DefaultValue) BandPass::reset_to(sample);
        if (_cfg.mode == Mode::RELATIVE) BandPass::update_limits();
        if (sample >= _lower && sample <= _upper) {
            // DBGF("Accepting value of %f", reading);
            _value = sample;
            _rejections = 0;
        } else {
            DBG("--------------------------------------------------------------------------");
            DBG("Bandpass rejected value of %f, limits: low: %f, high: %f", sample, _lower, _upper);
            DBG("--------------------------------------------------------------------------");
            if (++_rejections > _cfg.rejection_limit) {
                if (_cfg.throw_on_rejection_limit) {
                    spn::throw_exception(spn::runtime_exception("BandPass: rejection limit reached"));
                }
                _value = sample;
            }
        }
    }

    ValueType value() const override { return _value; }

    void reset_to(ValueType reading) override { _value = reading; }

    ValueType operator()(ValueType reading) { return value(reading); }

protected:
    // BandPass specific functions
    void update_limits() {
        ValueType compensated_midpoint = _cfg.mode == Mode::RELATIVE ? _value + _cfg.offset : _cfg.offset;
        _upper = compensated_midpoint + std::pow(_cfg.mantissa * 10, (_cfg.decades + 1));
        _lower = compensated_midpoint - std::pow(_cfg.mantissa * 10, (_cfg.decades + 1));

        // DBGF("Setting limts of %f to %f with midpoint %f offset %f and last value %f", _lower, _upper,
        //      compensated_midpoint, _cfg.offset, _value);
    }

protected:
    const ValueType DefaultValue = ValueType(-789.123456789);

private:
    const Config _cfg;

    ValueType _value = DefaultValue;

    ValueType _lower;
    ValueType _upper;

    int _rejections = 0;
};

} // namespace spn::filter
