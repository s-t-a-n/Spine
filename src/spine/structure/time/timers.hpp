#pragma once

#include "spine/platform/hal.hpp"
#include "spine/structure/units/si.hpp"
#include "spine/structure/vector.hpp"

namespace spn::core::time {

/// Keeps track of time since last check
class Timer {
public:
    static constexpr auto NoReset = false;

public:
    Timer(time_ms millis_epoch = HAL::millis()) : _last(millis_epoch){};

    /// Returns time since last poll. If reset is true, the timer will reset.
    time_ms time_since_last(bool reset = true) {
        time_ms time_expired = time_ms(HAL::millis()) - _last;
        if (reset) this->reset();
        return time_expired;
    }

    /// Reset the timer
    void reset(time_ms millis_epoch = HAL::millis()) { _last = millis_epoch; }

private:
    time_ms _last;
};

/// Keeps track of a certain point in the future
class AlarmTimer {
public:
    /// for `future`=time_ms(200) and `absolute`=false, have this expire at 200 ms from now
    /// for `absolute`=true, future is absolute length of time offset on `millis()`
    AlarmTimer(time_ms future, bool absolute = false) : _future(absolute ? future : HAL::millis() + future) {
        spn_assert(!absolute || future > HAL::millis());
    };

    /// Returns true if the timer has expired
    [[nodiscard]] bool expired() {
        if (_expired) {
            return true;
        }

        if (_future.raw<int32_t>() <= HAL::millis().raw<int32_t>()) {
            _expired = true;
            return true;
        }
        return false;
    }

    /// Returns the moment in the future were the timer will expire
    [[nodiscard]] time_ms future() const { return _future; }

    /// Returns the time until the timer will expire
    [[nodiscard]] time_ms time_from_now() const { return future() - HAL::millis(); }

private:
    time_ms _future;
    bool _expired = false;
};

/// Keeps track of a point in the future, continiously
class IntervalTimer {
public:
    IntervalTimer(time_ms sampling_interval, bool allow_catch_up = false)
        : _previous(HAL::millis()), _interval(sampling_interval), _allow_catch_up(allow_catch_up) {}

    /// Returns true exactly once if the timer has expired.
    [[nodiscard]] bool expired() {
        const time_ms current = HAL::millis();

        if (current - _previous > _interval) {
            if (_allow_catch_up) {
                _previous += _interval;
            } else {
                _previous = current;
            }
            return true;
        }
        return false;
    }

    /// Reset the timer.
    void reset() { _previous = HAL::millis(); }

    /// Returns the moment in the future when the timer will expire
    time_ms future() const { return _previous + _interval; }

    /// Returns the time until the timer will expire
    time_ms time_until_next() const { return future() - HAL::millis(); }

    /// Returns the interval of this timer
    time_ms interval() const { return _interval; }

private:
    time_ms _previous;
    time_ms _interval;
    bool _allow_catch_up;
};

} // namespace spn::core::time
