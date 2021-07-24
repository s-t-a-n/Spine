#pragma once

#include "spine/core/debugging.hpp"

#include <utility>

constexpr auto DefaultSensorValue = -1.234567890;

template<typename SensorSource, typename FilterType>
class Sensor {
public:
    struct Config {
        using Sensor = SensorSource;
        using Filter = FilterType;

        typename SensorSource::Config sensor_cfg;
        typename FilterType::Config filter_cfg;
    };

    Sensor(const Config&& cfg)
        : _cfg(std::move(cfg)), _src(std::move(cfg.sensor_cfg)), _flt(std::move(cfg.filter_cfg)) {
        //
    }

    void initialize() {
        _src.initialize();
        update();
    }

    void update() {
        const auto reading = _src.read();
        _value = _flt.value(reading);
    }

    double value() const {
        assert(_value != DefaultSensorValue);
        return _value;
    }

    double read() {
        update();
        return value();
    }

    SensorSource& src() { return _src; }
    FilterType& flt() { return _flt; }

private:
    const Config _cfg;

    SensorSource _src;
    FilterType _flt;

    double _value = DefaultSensorValue;
};