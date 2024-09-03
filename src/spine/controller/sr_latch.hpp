#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/time.hpp"
#include "spine/core/timers.hpp"

#include <cmath>

namespace spn::controller {

/// Set-Reset Latch
/// Triggers when value is above `high`, resets when value is below `low`
/// Independently of value, on and off time can be limited by time
/// Main purpose; controlling a relay with a hysteresis and a minimal ontime
class SRLatch {
public:
    using Timer = spn::core::time::Timer;

    struct Config {
        double high;
        double low;
        time_ms minimal_on_time = time_ms(0);
        time_ms maximal_on_time = time_ms(0);
        time_ms minimal_off_time = time_ms(0);
        time_ms maximal_off_time = time_ms(0);
    };

    using State = LogicalState;

    SRLatch(const Config&& cfg) : _cfg(std::move(cfg)), _last_turned(Timer(HAL::millis() - _cfg.minimal_off_time)) {}

    void initialize(){};

    void new_reading(double value) {
        if (_value == LogicalState::ON && _last_turned.timeSinceLast(Timer::NoReset) < _cfg.minimal_on_time) return;
        if (_value == LogicalState::OFF && _last_turned.timeSinceLast(Timer::NoReset) < _cfg.minimal_off_time) return;

        // If turned on longer than `maximal_on_time`, turn off
        if (_cfg.maximal_on_time != time_ms{} && _value == LogicalState::ON
            && _last_turned.timeSinceLast(Timer::NoReset) > _cfg.maximal_on_time) {
            set(State::OFF);
            return;
        }

        // If turned off longer than `maximal_off_time`, turn on
        if (_cfg.maximal_off_time != time_ms{} && _value == LogicalState::OFF
            && _last_turned.timeSinceLast(Timer::NoReset) > _cfg.maximal_off_time) {
            set(State::ON);
            return;
        }

        if (value > _cfg.high) {
            DBG("SRLatch triggered by high value of %f (lim: %f)", value, _cfg.high);
            set(State::ON);
        } else if (value < _cfg.low) {
            DBG("SRLatch triggered by low value of %f (lim: %f)", value, _cfg.low);
            set(State::OFF);
        }
    }
    bool response() const { return _value == State::ON; }

protected:
    void set(const State state) {
        _value = state;
        _last_turned.reset();
        DBG("Setting state: %i", state);
    }

private:
    const Config _cfg;

    State _value = State::OFF;

    Timer _last_turned;
};

} // namespace spn::controller