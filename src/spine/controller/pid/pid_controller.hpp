/**********************************************************************************************
 * Arduino PID Library - Version 1.2.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under the MIT License
 **********************************************************************************************/

// Downloaded on the 14th of May 2024 from https://github.com/br3ttb/Arduino-PID-Library

#include "spine/core/debugging.hpp"
#include "spine/core/time.hpp"
#include "spine/platform/hal.hpp"

namespace spn::controller {

class PIDController {
public:
    enum class Direction { FORWARD, REVERSE };
    enum class Proportionality { ON_MEASUREMENT, ON_ERROR };

    // commonly used functions **************************************************************************
    PIDController(double* const input, double* const output, double* const setpoint, const double Kp, const double Ki,
                  const double Kd, const Proportionality proportionality, const Direction controllerDirection)
        : _user_input(input), _user_output(output), _user_setpoint(setpoint) {
        // defaults
        PIDController::set_controller_direction(controllerDirection);
        PIDController::set_sampling_time(time_ms(100));
        PIDController::set_tunings(Kp, Ki, Kd, proportionality);
        PIDController::set_output_limits(0, 255);
        _last_time = HAL::millis() - _sampling_time;
    }

    PIDController(double* Input, double* Output, double* Setpoint, double Kp, double Ki, double Kd,
                  Direction ControllerDirection)
        : PIDController(Input, Output, Setpoint, Kp, Ki, Kd, Proportionality::ON_ERROR, ControllerDirection) {}

    void initialize() {
        _cumulative_output = *_user_output;
        _last_reading = *_user_input;
        if (_cumulative_output > _output_upper_limit) {
            _cumulative_output = _output_upper_limit;
        } else if (_cumulative_output < _output_lower_limit) {
            _cumulative_output = _output_lower_limit;
        }
    }

    bool update() {
        const auto now = HAL::millis();
        if (const auto timeChange = (now - _last_time); timeChange >= _sampling_time) {
            /*Compute all the working error variables*/
            const double input = *_user_input;
            const double error = *_user_setpoint - input;
            const double dInput = (input - _last_reading);
            _cumulative_output += (_ki * error);

            /*Add Proportional on Measurement, if P_ON_M is specified*/
            if (_proportionality == Proportionality::ON_MEASUREMENT) _cumulative_output -= _kp * dInput;

            if (_cumulative_output > _output_upper_limit) _cumulative_output = _output_upper_limit;
            else if (_cumulative_output < _output_lower_limit)
                _cumulative_output = _output_lower_limit;

            /*Add Proportional on Error, if P_ON_E is specified*/
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

    void set_output_limits(double Min, double Max) {
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

    void set_tunings(double Kp, double Ki, double Kd,
                     const Proportionality proportionality = Proportionality::ON_MEASUREMENT) {
        assert(Kp >= 0 && Ki >= 0 && Kd >= 0);
        assert(_sampling_time > time_ms{});

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

    void set_controller_direction(const Direction direction) {
        if (direction != _controller_direction) {
            _kp = (0 - _kp);
            _ki = (0 - _ki);
            _kd = (0 - _kd);
            _controller_direction = direction;
        }
    }

    void set_sampling_time(const time_ms sampling_time) {
        // adjust sample
        if (sampling_time > time_ms(0)) {
            double ratio = sampling_time.raw<double>() / _sampling_time.raw<double>();
            _ki *= ratio;
            _kd /= ratio;
            _sampling_time = sampling_time;
        }
    }

    double Kp() const { return _kp; }
    double Ki() const { return _ki; }
    double Kd() const { return _kd; }

    Direction direction() const { return _controller_direction; }

private:
    double _kp; // * (P)roportional Tuning Parameter
    double _ki; // * (I)ntegral Tuning Parameter
    double _kd; // * (D)erivative Tuning Parameter

    enum Direction _controller_direction;

    double* _user_input; // * Pointers to the Input, Output, and Setpoint variables
    double* _user_output; //   This creates a hard link between the variables and the
    double* _user_setpoint; //   PID, freeing the user from having to constantly tell us
    //   what these values are.  with pointers we'll just know.

    time_ms _last_time;
    double _cumulative_output;
    double _last_reading;

    time_ms _sampling_time;
    double _output_lower_limit;
    double _output_upper_limit;
    Proportionality _proportionality;
};

} // namespace spn::controller