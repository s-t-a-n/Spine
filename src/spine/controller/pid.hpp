#pragma once

#include "spine/controller/implementations/pid/pid_controller.hpp"
#include "spine/controller/implementations/pid/pid_tuner.hpp"
#include "spine/core/debugging.hpp"
#include "spine/core/logging.hpp"
#include "spine/structure/time/timers.hpp"

#include <cmath>
#include <functional>

namespace spn::controller {

class PID {
public:
    struct Tunings {
        float Kp;
        float Ki = 0;
        float Kd = 0;

        bool operator==(const Tunings& other) const { return Kp == other.Kp && Ki == other.Ki && Kd == other.Kd; }
    };

    using Direction = PIDController::Direction;
    using Proportionality = PIDController::Proportionality;

    struct Config {
        Tunings tunings = Tunings{.Kp = 20.0, .Ki = 0.3, .Kd = 0.1};

        float output_lower_limit = 0;
        float output_upper_limit = 255;
        k_time_ms sample_interval = k_time_ms(100);

        Proportionality proportionality = Proportionality::ON_ERROR;
        Direction direction = Direction::FORWARD;
    };

    struct TuneConfig {
        float setpoint;
        float startpoint = 0;
        float hysteresis = 0.01;
        bool satured_at_start = true;
        int cycles = 10;
        PIDAutotuner::Aggressiveness aggressiveness = PIDAutotuner::Aggressiveness::LessOvershoot;
    };

public:
    PID(Config&& cfg);
    PID(const Config& cfg) : PID(Config(cfg)) {}

    /// Initialize the PID-controller backend
    void initialize();

    /// Set the target setpoint
    void set_target_setpoint(float value) { _setpoint = value; }

    /// Set the controller's tuning parameters
    void set_tunings(const Tunings& tunings, const Proportionality proportionality = Proportionality::ON_ERROR) {
        _pid_backend.set_tunings(tunings.Kp, tunings.Ki, tunings.Kd, proportionality);
    }

    /// Returns the controller's tuning parameters
    Tunings tunings() const {
        return Tunings{.Kp = _pid_backend.Kp(), .Ki = _pid_backend.Ki(), .Kd = _pid_backend.Kd()};
    }

    /// Set latest reading (update happens based on system clock)
    void new_reading(float value);

    /// Set latest reading with arbitrary now (which is used to calculate time to update)
    void new_reading(float value, k_time_ms now);

    /// Autotune the proportional weights for the target_setpoint
    Tunings autotune(const TuneConfig& tune_config, std::function<void(float)> process_setter,
                     std::function<float(void)> process_getter, std::function<void(void)> loop = {},
                     std::function<k_time_ms(void)> uptime = HAL::millis,
                     std::function<void(k_time_ms)> sleep = HAL::delay) const;

    /// Get the controller response
    float response() const {
        spn_assert(!std::isnan(_output));
        return _output;
    }

    /// Get the error between the setpoint and input
    float error() const { return std::fabs(_setpoint - _input); }

    /// Get the setpoint
    float setpoint() const { return _setpoint; }

private:
    Config _cfg;

    PIDController _pid_backend;

    float _setpoint = 0;
    float _input = 0;
    float _output = 0;
};

} // namespace spn::controller
