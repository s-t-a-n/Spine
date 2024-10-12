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
        double Kp;
        double Ki = 0;
        double Kd = 0;

        bool operator==(const Tunings& other) const { return Kp == other.Kp && Ki == other.Ki && Kd == other.Kd; }
    };

    using Direction = PIDController::Direction;
    using Proportionality = PIDController::Proportionality;

    struct Config {
        Tunings tunings = Tunings{.Kp = 20.0, .Ki = 0.3, .Kd = 0.1};

        double output_lower_limit = 0;
        double output_upper_limit = 255;
        time_ms sample_interval = time_ms(100);

        Proportionality proportionality = Proportionality::ON_ERROR;
        Direction direction = Direction::FORWARD;
    };

    struct TuneConfig {
        double setpoint;
        double startpoint = 0;
        double hysteresis = 0.01;
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
    void set_target_setpoint(double value) { _setpoint = value; }

    /// Set the controller's tuning parameters
    void set_tunings(const Tunings& tunings, const Proportionality proportionality = Proportionality::ON_ERROR) {
        _pid_backend.set_tunings(tunings.Kp, tunings.Ki, tunings.Kd, proportionality);
    }

    /// Returns the controller's tuning parameters
    Tunings tunings() const {
        return Tunings{.Kp = _pid_backend.Kp(), .Ki = _pid_backend.Ki(), .Kd = _pid_backend.Kd()};
    }

    /// Set latest reading
    void new_reading(double value);

    /// Autotune the proportional weights for the target_setpoint
    Tunings autotune(const TuneConfig& tune_config, std::function<void(double)> process_setter,
                     std::function<double(void)> process_getter, std::function<void(void)> loop = {}) const;

    /// Get the controller response
    double response() const {
        spn_assert(!std::isnan(_output));
        return _output;
    }

    /// Get the error between the setpoint and input
    double error() const { return std::fabs(_setpoint - _input); }

    /// Get the setpoint
    double setpoint() const { return _setpoint; }

private:
    Config _cfg;

    PIDController _pid_backend;

    double _setpoint = 0;
    double _input = 0;
    double _output = 0;
};

} // namespace spn::controller
