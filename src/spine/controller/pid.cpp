#include "spine/controller/pid.hpp"

namespace spn::controller {

PID::PID(PID::Config&& cfg)
    : _cfg(cfg),
      _pid_backend(PIDController(&_input, &_output, &_setpoint, _cfg.tunings.Kp, _cfg.tunings.Ki, _cfg.tunings.Kd,
                                 PIDController::Proportionality::ON_MEASUREMENT, _cfg.direction)) {
    spn_assert(k_time_ms(_cfg.sample_interval).raw() > 0);
}

void PID::initialize() {
    _pid_backend.set_output_limits(_cfg.output_lower_limit, _cfg.output_upper_limit);
    _pid_backend.set_sampling_time(_cfg.sample_interval);
    _pid_backend.initialize();
}

void PID::new_reading(double value) { new_reading(value, HAL::millis()); }

void PID::new_reading(double value, k_time_ms now) {
    _input = value;
    _pid_backend.update(now);
}

PID::Tunings PID::autotune(const PID::TuneConfig& tune_config, std::function<void(double)> process_setter,
                           std::function<double(void)> process_getter, std::function<void(void)> loop,
                           std::function<k_time_ms()> uptime, std::function<void(k_time_ms)> sleep) const {
    SPN_LOG("Starting autotune");

    using spn::structure::time::AlarmTimer;
    const auto block_until_setpoint = [&](const double setpoint, k_time_ms timeout = k_time_ms(0),
                                          bool saturated = false) {
        auto timer = AlarmTimer(timeout);
        while (process_getter() < setpoint && (!timer.expired() || timeout == k_time_ms(0))) {
            process_setter(_cfg.output_upper_limit);
            if (loop) loop();
            SPN_DBG("Waiting until temperature of %.2f C reaches %.2f C, saturating thermal capacitance",
                    process_getter(), setpoint);
            sleep(k_time_ms(1000));
        }
        process_setter(_cfg.output_lower_limit);
        if (loop) loop();
        if (saturated) return;
        while (process_getter() > setpoint && (!timer.expired() || timeout == k_time_ms(0))) {
            SPN_DBG("Waiting until temperature of %.2f C reaches %.2f C, unloading thermal capacitance",
                    process_getter(), setpoint);
            sleep(k_time_ms(1000));
        }
    };

    if (tune_config.startpoint > 0) {
        block_until_setpoint(tune_config.startpoint, k_time_m(30), true);
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

    SPN_LOG("Starting autotuning loop for %i cycles", tune_config.cycles);

    // This must be called immediately before the tuning loop
    // Must be called with the current time in microseconds
    tuner.start_tuning_loop(uptime());

    // Run a loop until tuner.isFinished() returns true
    auto previous_cycle = 0;
    auto previous_cycle_start = uptime();
    while (!tuner.is_finished()) {
        // This loop must run at the same speed as the PID control loop being tuned
        k_time_ms iteration_start = uptime();

        if (const auto cycle = tuner.get_cycle(); cycle == previous_cycle + 1) {
            previous_cycle++;
            const auto duration = uptime() - previous_cycle_start;
            [[maybe_unused]] const k_time_ms remaining = duration * (tune_config.cycles - cycle);
            SPN_LOG("Cycle %i complete in %u seconds, remaining: %u minutes", cycle, k_time_s(duration).raw(),
                    k_time_m(remaining).raw());
            previous_cycle_start = uptime();
        }

        // Get input value here (temperature, encoder position, velocity, etc)
        const double process_value = process_getter();

        // Call tunePID() with the input value and current time in microseconds
        const double control_value = tuner.do_autotune(process_value, iteration_start);

        SPN_DBG("Cycle %i/%i: PV: %f, CV: %f", tuner.get_cycle(), tune_config.cycles, process_value, control_value);

        // Set the output - tunePid() will return values within the range configured
        // by setOutputRange(). Don't change the value or the tuning results will be
        // incorrect.
        spn_assert(control_value >= _cfg.output_lower_limit);
        spn_assert(control_value <= _cfg.output_upper_limit);
        process_setter(control_value);

        // This loop must run at the same speed as the PID control loop being tuned
        const auto interval = k_time_ms(1);
        while (uptime() - iteration_start < _cfg.sample_interval) {
            if (loop) {
                loop();
            }
            sleep(interval);
        }
    }
    process_setter(0); // Turn the output off.
    return Tunings{.Kp = tuner.get_Kp(), .Ki = tuner.get_Ki(), .Kd = tuner.get_Kd()};
}

} // namespace spn::controller