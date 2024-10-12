#include "spine/structure/time/timers.hpp"

namespace spn::structure::time {

Timer::Timer(time_ms millis_epoch) : _last(millis_epoch) {}

time_ms Timer::time_since_last(bool reset) {
    time_ms time_expired = time_ms(HAL::millis()) - _last;
    if (reset) this->reset();
    return time_expired;
}

void Timer::reset(time_ms millis_epoch) { _last = millis_epoch; }

/*
 * TIMER: ALARM
 */

AlarmTimer::AlarmTimer(time_ms future, bool absolute) : _future(absolute ? future : HAL::millis() + future) {
    spn_assert(!absolute || future > HAL::millis());
}

bool AlarmTimer::expired() {
    if (_expired) {
        return true;
    }

    if (_future.raw<int32_t>() <= HAL::millis().raw<int32_t>()) {
        _expired = true;
        return true;
    }
    return false;
}

time_ms AlarmTimer::time_from_now() const { return future() - HAL::millis(); }

/*
 * TIMER: INTERVAL
 */

IntervalTimer::IntervalTimer(time_ms sampling_interval, bool allow_catch_up)
    : _previous(HAL::millis()), _interval(sampling_interval), _allow_catch_up(allow_catch_up) {}

bool IntervalTimer::expired() {
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

void IntervalTimer::reset() { _previous = HAL::millis(); }

time_ms IntervalTimer::future() const { return _previous + _interval; }

time_ms IntervalTimer::time_until_next() const { return future() - HAL::millis(); }
} // namespace spn::structure::time