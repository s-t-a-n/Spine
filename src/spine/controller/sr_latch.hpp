#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/types.hpp"
#include "spine/structure/time/timers.hpp"
#include "spine/structure/units/si.hpp"

namespace spn::controller {

/// Set-Reset Latch
/// Triggers when value is above `high`, resets when value is below `low`
/// Independently of value, on and off time can be limited by time
/// Main purpose; controlling a relay with a hysteresis and a minimal ontime
class SRLatch {
public:
    using Timer = spn::structure::time::Timer;

    struct Config {
        double high;
        double low;
        time_ms minimal_on_time = time_ms{};
        time_ms maximal_on_time = time_ms{};
        time_ms minimal_off_time = time_ms{};
        time_ms maximal_off_time = time_ms{};
    };

    using State = core::LogicalState;

    SRLatch(const Config&& cfg) : _cfg(std::move(cfg)), _last_turned(Timer(HAL::millis() - _cfg.minimal_off_time)) {}

    void initialize(){}; // boilerplate

    void new_reading(double value);
    bool response() const { return _value == State::ON; }

protected:
    void set(const State state) {
        _value = state;
        _last_turned.reset();
    }

private:
    const Config _cfg;

    State _value = State::OFF;
    Timer _last_turned;
};

} // namespace spn::controller
