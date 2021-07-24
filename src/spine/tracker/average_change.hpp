#pragma once

#include <spine/filter/implementations/ewma.hpp>

namespace spn::tracker {

using spn::filter::EWMA;

template<typename T>
class AverageChange {
public:
    struct Config {
        size_t last_n_elements;
    };

    AverageChange(const Config&& cfg)
        : _cfg(std::move(cfg)), _last_element({}), _ma(EWMA<T>::Config{.K = _cfg.last_n_elements}) {
        _ma.reset_to({});
    }

    T avg_chance(T new_reading) {
        _ma.new_sample(std::fabs(new_reading - _last_element));
        _last_element = new_reading;
        return _ma.value();
    }

private:
    const Config _cfg;

    T _last_element = {};
    EWMA<T> _ma;
};
} // namespace spn::tracker