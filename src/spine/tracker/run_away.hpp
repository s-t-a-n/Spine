#pragma once

/// Track closed-loop processes for behaving in their expected 'glideslope'
/// A plane's expected glideslope is based on it's pitch and engine throttle
/// When the engine throttle is (fully) on and the pitch is up but the plane doesn't rise, the process has 'run away'
class RunAwayTracker {
public:
    enum class State { UNDEFINED, RISING, FALLING, STABLE, RUNAWAY };

    struct Config {
        double expected_rise_at_full_throttle;
    };

    RunAwayTracker(const Config& cfg) : _cfg(cfg) {}

    void update(double pitch, double throttle, double altitude) {}

private:
    const Config _cfg;
};