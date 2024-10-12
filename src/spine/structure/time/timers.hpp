#pragma once

#include "spine/platform/hal.hpp"
#include "spine/structure/units/si.hpp"
#include "spine/structure/vector.hpp"

namespace spn::structure::time {

/// Keeps track of time since last check
class Timer {
public:
    static constexpr auto NoReset = false;

public:
    Timer(time_ms millis_epoch = HAL::millis());
    ;

    /// Returns time since last poll. If reset is true, the timer will reset.
    time_ms time_since_last(bool reset = true);

    /// Reset the timer
    void reset(time_ms millis_epoch = HAL::millis());

private:
    time_ms _last;
};

/// Keeps track of a certain point in the future
class AlarmTimer {
public:
    /// for `future`=time_ms(200) and `absolute`=false, have this expire at 200 ms from now
    /// for `absolute`=true, future is absolute length of time offset on `millis()`
    AlarmTimer(time_ms future, bool absolute = false);
    ;

    /// Returns true if the timer has expired
    [[nodiscard]] bool expired();

    /// Returns the moment in the future were the timer will expire
    [[nodiscard]] time_ms future() const { return _future; }

    /// Returns the time until the timer will expire
    [[nodiscard]] time_ms time_from_now() const;

private:
    time_ms _future;
    bool _expired = false;
};

/// Keeps track of a point in the future, continiously
class IntervalTimer {
public:
    IntervalTimer(time_ms sampling_interval, bool allow_catch_up = false);

    /// Returns true exactly once if the timer has expired.
    [[nodiscard]] bool expired();

    /// Reset the timer.
    void reset();

    /// Returns the moment in the future when the timer will expire
    time_ms future() const;

    /// Returns the time until the timer will expire
    time_ms time_until_next() const;

    /// Returns the interval of this timer
    time_ms interval() const { return _interval; }

private:
    time_ms _previous;
    time_ms _interval;
    bool _allow_catch_up;
};

} // namespace spn::structure::time
