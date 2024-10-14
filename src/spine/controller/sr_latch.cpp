#include "spine/controller/sr_latch.hpp"

namespace spn::controller {

void SRLatch::new_reading(double value) {
    if (_value == State::ON && _last_turned.time_since_last(Timer::NoReset) < _cfg.minimal_on_time) return;
    if (_value == State::OFF && _last_turned.time_since_last(Timer::NoReset) < _cfg.minimal_off_time) return;

    // If turned on longer than `maximal_on_time`, turn off
    if (_cfg.maximal_on_time != k_time_ms{} && _value == State::ON
        && _last_turned.time_since_last(Timer::NoReset) > _cfg.maximal_on_time) {
        set(State::OFF);
        return;
    }

    // If turned off longer than `maximal_off_time`, turn on
    if (_cfg.maximal_off_time != k_time_ms{} && _value == State::OFF
        && _last_turned.time_since_last(Timer::NoReset) > _cfg.maximal_off_time) {
        set(State::ON);
        return;
    }

    if (value > _cfg.high) {
        SPN_DBG("SRLatch: triggered by high value of %f (lim: %f)", value, _cfg.high);
        set(State::ON);
    } else if (value < _cfg.low) {
        SPN_DBG("SRLatch: triggered by low value of %f (lim: %f)", value, _cfg.low);
        set(State::OFF);
    }
}
} // namespace spn::controller