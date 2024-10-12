#include "spine/controller/implementations/pid/pid_controller.hpp"

namespace spn::controller {

PIDController::PIDController(double* const input, double* const output, double* const setpoint, const double Kp,
                             const double Ki, const double Kd, const PIDController::Proportionality proportionality,
                             const PIDController::Direction controllerDirection)
    : _user_input(input), _user_output(output), _user_setpoint(setpoint) {
    spn_assert(input);
    spn_assert(output);
    spn_assert(setpoint);
    PIDController::set_controller_direction(controllerDirection);
    PIDController::set_tunings(Kp, Ki, Kd, proportionality);
    _last_time = HAL::millis() - _sampling_time;
}

PIDController::PIDController(double* Input, double* Output, double* Setpoint, double Kp, double Ki, double Kd,
                             PIDController::Direction ControllerDirection)
    : PIDController(Input, Output, Setpoint, Kp, Ki, Kd, Proportionality::ON_ERROR, ControllerDirection) {}
void PIDController::initialize() {
    _cumulative_output = *_user_output;
    _last_reading = *_user_input;
    if (_cumulative_output > _output_upper_limit) {
        _cumulative_output = _output_upper_limit;
    } else if (_cumulative_output < _output_lower_limit) {
        _cumulative_output = _output_lower_limit;
    }
}

bool PIDController::update() {
    const auto now = HAL::millis();
    if (const auto timeChange = (now - _last_time); timeChange >= _sampling_time) {
        /*Compute all the working error variables*/
        const double input = *_user_input;
        const double error = *_user_setpoint - input;
        const double dInput = (input - _last_reading);
        _cumulative_output += (_ki * error);

        /*Add Proportional on Measurement, if ON_MEASUREMENT is specified*/
        if (_proportionality == Proportionality::ON_MEASUREMENT) _cumulative_output -= _kp * dInput;

        if (_cumulative_output > _output_upper_limit) _cumulative_output = _output_upper_limit;
        else if (_cumulative_output < _output_lower_limit)
            _cumulative_output = _output_lower_limit;

        /*Add Proportional on Error, if ON_ERROR is specified*/
        double output;
        if (_proportionality == Proportionality::ON_ERROR) output = _kp * error;
        else
            output = 0;

        /*Compute Rest of PID Output*/
        output += _cumulative_output - _kd * dInput;

        if (output > _output_upper_limit) output = _output_upper_limit;
        else if (output < _output_lower_limit)
            output = _output_lower_limit;
        *_user_output = output;

        /*Remember some variables for next time*/
        _last_reading = input;
        _last_time = now;
        return true;
    }
    return false;
}

void PIDController::set_output_limits(double Min, double Max) {
    if (Min >= Max) return;
    _output_lower_limit = Min;
    _output_upper_limit = Max;

    if (*_user_output > _output_upper_limit) *_user_output = _output_upper_limit;
    else if (*_user_output < _output_lower_limit)
        *_user_output = _output_lower_limit;

    if (_cumulative_output > _output_upper_limit) _cumulative_output = _output_upper_limit;
    else if (_cumulative_output < _output_lower_limit)
        _cumulative_output = _output_lower_limit;
}

void PIDController::set_tunings(double Kp, double Ki, double Kd, const PIDController::Proportionality proportionality) {
    spn_assert(Kp >= 0 && Ki >= 0 && Kd >= 0);
    spn_assert(_sampling_time > time_ms{});

    _proportionality = proportionality;

    const auto sampletime_in_seconds = _sampling_time.raw<double>() / 1000.0;
    _kp = Kp;
    _ki = Ki * sampletime_in_seconds;
    _kd = Kd / sampletime_in_seconds;

    if (_controller_direction == Direction::REVERSE) {
        _kp = (0 - _kp);
        _ki = (0 - _ki);
        _kd = (0 - _kd);
    }
}

void PIDController::set_controller_direction(const PIDController::Direction direction) {
    if (direction != _controller_direction) {
        _kp = (0 - _kp);
        _ki = (0 - _ki);
        _kd = (0 - _kd);
        _controller_direction = direction;
    }
}

void PIDController::set_sampling_time(const time_ms sampling_time) {
    // adjust sample
    if (sampling_time > time_ms(0)) {
        double ratio = sampling_time.raw<double>() / _sampling_time.raw<double>();
        _ki *= ratio;
        _kd /= ratio;
        _sampling_time = sampling_time;
    }
}

} // namespace spn::controller