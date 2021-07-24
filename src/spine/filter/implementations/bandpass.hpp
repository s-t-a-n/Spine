#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <spine/platform/hal.hpp>

#include <cmath>
#include <cstdint>
#include <utility>

namespace spn::filter {

template<typename ValueType = double>
/// Filter that accepts new values within a logarithmic distance of the last value
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

    /// Returns a Broadband filter
    static std::unique_ptr<BandPass> Broad() {
        return std::make_unique<BandPass>(Config{.mode = Mode::RELATIVE,
                                                 .mantissa = 1,
                                                 .decades = 0.01,
                                                 .offset = 0,
                                                 .rejection_limit = 10,
                                                 .throw_on_rejection_limit = true});
    }

    BandPass(const Config&& cfg) : _cfg(std::move(cfg)) {
        static_assert(std::is_floating_point_v<ValueType>, "ValueType must be a floating point type");
        assert(_cfg.mantissa >= 1.0 && _cfg.mantissa < 10.0); // sanity
        BandPass::update_limits();
    }

    /// Provide a new sample for filtering (optionally throws if value is out of band)
    void new_sample(const ValueType sample) override {
        if (_value == DefaultValue) BandPass::reset_to(sample);
        if (_cfg.mode == Mode::RELATIVE) BandPass::update_limits();

        if (sample >= _lower && sample <= _upper) {
            _value = sample;
            _rejections = 0;
            return;
        }

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

    /// Take a sample, returns the filtered value
    ValueType value(const ValueType sample) override {
        new_sample(sample);
        return _value;
    }

    /// Returns the filtered value
    ValueType value() const override { return _value; }
    ValueType operator()(ValueType reading) { return value(reading); }

    /// Reset the filter's internal value to a provided value (next sample will be filtered based on this value)
    void reset_to(ValueType reading) override { _value = reading; }

protected:
    void update_limits() {
        ValueType compensated_midpoint = _cfg.mode == Mode::RELATIVE ? _value + _cfg.offset : _cfg.offset;
        _upper = compensated_midpoint + std::pow(_cfg.mantissa * 10, (_cfg.decades + 1));
        _lower = compensated_midpoint - std::pow(_cfg.mantissa * 10, (_cfg.decades + 1));
    }

    /// Arbitrary value to be used as a default value
    const ValueType DefaultValue = ValueType(-789.123456789);

private:
    const Config _cfg;

    ValueType _value = DefaultValue;
    ValueType _lower;
    ValueType _upper;
    int _rejections = 0;
};

} // namespace spn::filter
