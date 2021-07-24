#pragma once

#include "spine/core/time.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/vector.hpp"

namespace spn::core::time {

/// keeps track of time since last check
class Timer {
public:
    static constexpr auto NoReset = false;

public:
    Timer(time_ms millis_epoch = HAL::millis()) : _last(millis_epoch){};

    time_ms timeSinceLast(bool reset = true) {
        time_ms time_expired = time_ms(HAL::millis()) - _last;
        if (reset) this->reset();
        return time_expired;
    }
    void reset(time_ms millis_epoch = HAL::millis()) { _last = millis_epoch; }

private:
    time_ms _last;
};

/// keeps track of a certain point in the future
class AlarmTimer {
public:
    /// for `future`=time_ms(200) and `absolute`=false, have this expire at 200 ms from now
    /// for `absolute`=true, future is absolute length of time offset on `millis()`
    AlarmTimer(time_ms future, bool absolute = false) : _future(absolute ? future : HAL::millis() + future) {
        assert(!absolute || future > HAL::millis());
    };

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

    [[nodiscard]] time_ms future() const { return _future; }
    [[nodiscard]] time_ms time_from_now() const { return future() - HAL::millis(); }

private:
    time_ms _future;
    bool _expired = false;
};

// keeps track of a point in the future, continiously
class IntervalTimer {
public:
    IntervalTimer(time_ms sampling_interval, bool allow_catch_up = false)
        : _previous(HAL::millis()), _sampling_interval(sampling_interval), _allow_catch_up(allow_catch_up) {}

    // it is expected that when the timer expires, the consumer handles their business
    [[nodiscard]] bool expired() {
        const time_ms current = HAL::millis();

        if (current - _previous > _sampling_interval) {
            if (_allow_catch_up) {
                _previous += _sampling_interval;
            } else {
                _previous = current;
            }
            return true;
        }
        return false;
    }

    void reset() { _previous = HAL::millis(); }

    time_ms future() const { return _previous + _sampling_interval; }
    time_ms time_until_next() const { return future() - HAL::millis(); }
    time_ms sampling_interval() const { return _sampling_interval; }

private:
    time_ms _previous;
    const time_ms _sampling_interval;
    const bool _allow_catch_up;
};

} // namespace spn::core::time
