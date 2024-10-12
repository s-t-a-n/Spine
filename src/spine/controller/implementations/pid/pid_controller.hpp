/*********************************************************************************************
 * Arduino PID Library - Version 1.2.1
 *
 * This library was originally created by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 * and licensed under the MIT License
 *
 * Downloaded on 14-05-2024 from https://github.com/br3ttb/Arduino-PID-Library
 *
 * Modifications by Stan:
 * - naming made consistent within Spine
 * - use Spine's time units
 * - math is mostly consistent with original, deduplicated some functions
 * - remove unnecessary functionality
 **********************************************************************************************/

#include "spine/core/debugging.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/units/si.hpp"

namespace spn::controller {

class PIDController {
public:
    enum class Direction { FORWARD, REVERSE };
    enum class Proportionality { ON_MEASUREMENT, ON_ERROR };

    // commonly used functions **************************************************************************
    PIDController(double* const input, double* const output, double* const setpoint, const double Kp, const double Ki,
                  const double Kd, const Proportionality proportionality, const Direction controllerDirection);

    PIDController(double* Input, double* Output, double* Setpoint, double Kp, double Ki, double Kd,
                  Direction ControllerDirection);

    void initialize();

    bool update();

    void set_output_limits(double Min, double Max);

    void set_tunings(double Kp, double Ki, double Kd,
                     const Proportionality proportionality = Proportionality::ON_MEASUREMENT);

    void set_controller_direction(const Direction direction);

    void set_sampling_time(const time_ms sampling_time);

    double Kp() const { return _kp; }
    double Ki() const { return _ki; }
    double Kd() const { return _kd; }

    Direction direction() const { return _controller_direction; }

private:
    double _kp = 0; // * (P)roportional Tuning Parameter
    double _ki = 0; // * (I)ntegral Tuning Parameter
    double _kd = 0; // * (D)erivative Tuning Parameter

    enum Direction _controller_direction = Direction::FORWARD;

    double* _user_input = nullptr; // * Pointers to the Input, Output, and Setpoint variables
    double* _user_output = nullptr; //   This creates a hard link between the variables and the
    double* _user_setpoint = nullptr; //   PID, freeing the user from having to constantly tell us
    //   what these values are.  with pointers we'll just know.

    time_ms _last_time{};
    double _cumulative_output = 0;
    double _last_reading = 0;

    time_ms _sampling_time = time_ms{100};
    double _output_lower_limit = 0;
    double _output_upper_limit = 255;
    Proportionality _proportionality = Proportionality::ON_MEASUREMENT;
};

} // namespace spn::controller
