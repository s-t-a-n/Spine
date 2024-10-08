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
    };

    using Direction = PIDController::Direction;

    struct Config {
        Tunings tunings = Tunings{.Kp = 20.0, .Ki = 0.3, .Kd = 0.1};

        double output_lower_limit = 0;
        double output_upper_limit = 255;
        time_ms sample_interval = time_ms(100);
        PIDController::Direction direction = PIDController::Direction::FORWARD;
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
    PID(Config&& cfg)
        : _cfg(cfg),
          _pid_backend(PIDController(&_input, &_output, &_setpoint, _cfg.tunings.Kp, _cfg.tunings.Ki, _cfg.tunings.Kd,
                                     PIDController::Proportionality::ON_MEASUREMENT, _cfg.direction)) {
        assert(time_ms(_cfg.sample_interval).raw<>() > 0);
    }
    PID(const Config& cfg) : PID(Config(cfg)) {}

    /// Initialize the PID-controller backend
    void initialize() {
        _pid_backend.set_output_limits(_cfg.output_lower_limit, _cfg.output_upper_limit);
        _pid_backend.set_sampling_time(_cfg.sample_interval);
        _pid_backend.initialize();
    }

    /// Set the target setpoint
    void set_target_setpoint(double value) { _setpoint = value; }

    /// Set the controller's tuning parameters
    void set_tunings(const Tunings& tunings) { _pid_backend.set_tunings(tunings.Kp, tunings.Ki, tunings.Kd); }

    /// Returns the controller's tuning parameters
    Tunings tunings() const {
        return Tunings{.Kp = _pid_backend.Kp(), .Ki = _pid_backend.Ki(), .Kd = _pid_backend.Kd()};
    }

    /// Set latest reading
    void new_reading(double value) {
        _input = value;
        _pid_backend.update();
    }

    /// Autotune the proportional weights for the target_setpoint
    Tunings autotune(const TuneConfig& tune_config, std::function<void(double)> process_setter,
                     std::function<double(void)> process_getter, std::function<void(void)> loop = {}) {
        LOG("Starting autotune");

        using spn::core::time::AlarmTimer;
        const auto block_until_setpoint = [&](const double setpoint, time_ms timeout = time_ms(0),
                                              bool saturated = false) {
            auto timer = AlarmTimer(timeout);
            while (process_getter() < setpoint && (!timer.expired() || timeout == time_ms(0))) {
                process_setter(_cfg.output_upper_limit);
                if (loop) loop();
                DBG("Waiting until temperature of %.2f C reaches %.2f C, saturating thermal capacitance",
                    process_getter(), setpoint);
                HAL::delay(time_ms(1000));
            }
            process_setter(_cfg.output_lower_limit);
            if (loop) loop();
            if (saturated) return;
            while (process_getter() > setpoint && (!timer.expired() || timeout == time_ms(0))) {
                DBG("Waiting until temperature of %.2f C reaches %.2f C, unloading thermal capacitance",
                    process_getter(), setpoint);
                HAL::delay(time_ms(1000));
            }
        };

        if (tune_config.startpoint > 0) {
            block_until_setpoint(tune_config.startpoint, time_m(30), true);
        }

        PIDAutotuner tuner = PIDAutotuner();

        // Set the target value to tune to
        // This will depend on what you are tuning. This should be set to a value within
        // the usual range of the setpoint. For low-inertia systems, values at the lower
        // end of this range usually give better results. For anything else, start with a
        // value at the middle of the range.
        tuner.set_target_input_value(tune_config.setpoint);

        // Set the loop interval in microseconds
        // This must be the same as the interval the PID control loop will run at
        tuner.set_loop_interval(_cfg.sample_interval);

        // Set the output range
        // These are the minimum and maximum possible output values of whatever you are
        // using to control the system (Arduino analogWrite, for example, is 0-255)
        tuner.setOutputRange(_cfg.output_lower_limit, _cfg.output_upper_limit);

        // Make sure that an end of cycle can not be falsely triggered by the bouncing of a reading between discrete
        // levels
        tuner.set_hysteresis(tune_config.hysteresis);

        // Set the Ziegler-Nichols tuning mode
        // Set it to either PIDAutotuner::ZNModeBasicPID, PIDAutotuner::ZNModeLessOvershoot,
        // or PIDAutotuner::ZNModeNoOvershoot. Defaults to ZNModeNoOvershoot as it is the
        // safest option.
        tuner.set_aggressiveness(tune_config.aggressiveness);

        tuner.set_tuning_cycles(tune_config.cycles);

        LOG("Starting autotuning loop for %i cycles", tune_config.cycles);

        // This must be called immediately before the tuning loop
        // Must be called with the current time in microseconds
        tuner.start_tuning_loop(HAL::millis());

        // Run a loop until tuner.isFinished() returns true
        auto previous_cycle = 0;
        auto previous_cycle_start = HAL::millis();
        while (!tuner.is_finished()) {
            // This loop must run at the same speed as the PID control loop being tuned
            time_ms iteration_start = HAL::millis();

            if (const auto cycle = tuner.get_cycle(); cycle == previous_cycle + 1) {
                previous_cycle++;
                const auto duration = HAL::millis() - previous_cycle_start;
                [[maybe_unused]] const time_ms remaining = (tune_config.cycles - cycle) * duration;
                DBG("Cycle %i complete in %u seconds, remaining: %u minutes", cycle, time_s(duration).printable(),
                    time_m(remaining).printable());
                previous_cycle_start = HAL::millis();
            }

            // Get input value here (temperature, encoder position, velocity, etc)
            const double process_value = process_getter();

            // Call tunePID() with the input value and current time in microseconds
            const double control_value = tuner.do_autotune(process_value, iteration_start);

            LOG("Cycle %i/%i: PV: %f, CV: %f", tuner.get_cycle(), tune_config.cycles, process_value, control_value);

            // Set the output - tunePid() will return values within the range configured
            // by setOutputRange(). Don't change the value or the tuning results will be
            // incorrect.
            assert(control_value >= _cfg.output_lower_limit);
            assert(control_value <= _cfg.output_upper_limit);
            process_setter(control_value);

            // This loop must run at the same speed as the PID control loop being tuned
            const auto interval = time_ms(1);
            while (HAL::millis() - iteration_start < _cfg.sample_interval) {
                if (loop) {
                    loop();
                }
                HAL::delay(interval);
            }
        }
        process_setter(0); // Turn the output off.
        return Tunings{.Kp = tuner.get_Kp(), .Ki = tuner.get_Ki(), .Kd = tuner.get_Kd()};
    }

    /// Get the controller response
    double response() const {
        assert(!std::isnan(_output));
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