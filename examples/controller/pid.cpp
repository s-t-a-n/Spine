#include <spine/controller/pid.hpp>

double get_sensor_reading() {
    return 50.0; // get some sensor reading
}
void set_actuator_pv(double value) {
    (void)value; // set some actuator
}

int main() {
    using namespace spn::controller;

    // CREATE
    const auto pid_tunings = PID::Tunings{.Kp = 20, .Ki = 0.3, .Kd = 0}; // PI-controller
    const auto pid_cfg = PID::Config{.tunings = pid_tunings,
                                     .output_lower_limit = 0,
                                     .output_upper_limit = 255,
                                     .sample_interval = time_ms(100),
                                     .proportionality = PID::Proportionality::ON_ERROR,
                                     .direction = PID::Direction::FORWARD};
    auto pid = PID(pid_cfg);

    // INITIALIZE
    const auto setpoint = 100.0;
    pid.initialize();
    pid.set_target_setpoint(setpoint);

    // CONTROL
    pid.new_reading(get_sensor_reading());
    set_actuator_pv(pid.response());

    // AUTOTUNE
    const auto process_getter = [&]() { return get_sensor_reading(); };
    const auto process_setter = [&](double value) { set_actuator_pv(value); };
    const auto process_loop = [&]() { /* run necessary loops to have hardware function. Runs every millisecond */ };
    auto tune_cfg = PID::TuneConfig{.setpoint = 100.0,
                                    .startpoint = 90.0,
                                    .hysteresis = 0.01,
                                    .satured_at_start = true,
                                    .cycles = 30,
                                    .aggressiveness = PIDAutotuner::Aggressiveness::LessOvershoot};
    auto new_tunings = pid.autotune(tune_cfg, process_setter, process_getter, process_loop);
    pid.set_tunings(new_tunings);

    return 0;
}
