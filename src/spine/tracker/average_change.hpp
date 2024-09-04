#pragma once

#include "spine/filter/implementations/ewma.hpp"

namespace spn::tracker {

using spn::filter::EWMA;

template<typename ValueType = double>
class AverageChange {
public:
    struct Config {
        size_t last_n_elements;
    };

    AverageChange(const Config&& cfg)
        : _cfg(std::move(cfg)), _last_element({}), _ma(EWMA<>::Config{.K = _cfg.last_n_elements}) {
        _ma.reset_to({});
    }

    ValueType avg_chance(ValueType new_reading) {
        _ma.new_sample(std::fabs(new_reading - _last_element));
        _last_element = new_reading;
        return _ma.value();
    }

private:
    const Config _cfg;

    ValueType _last_element = {};
    EWMA<ValueType> _ma;
};
} // namespace spn::tracker