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

#include "spine/structure/units/time.hpp"

namespace spn::controller {

class PIDController {
public:
    enum class Direction { FORWARD, REVERSE };
    enum class Proportionality { ON_MEASUREMENT, ON_ERROR };

    // commonly used functions **************************************************************************
    PIDController(float* const input, float* const output, float* const setpoint, const float Kp, const float Ki,
                  const float Kd, const Proportionality proportionality, const Direction controllerDirection);

    PIDController(float* Input, float* Output, float* Setpoint, float Kp, float Ki, float Kd,
                  Direction ControllerDirection);

    void initialize();

    bool update(const k_time_ms now);

    void set_output_limits(float Min, float Max);

    void set_tunings(float Kp, float Ki, float Kd,
                     const Proportionality proportionality = Proportionality::ON_MEASUREMENT);

    void set_controller_direction(const Direction direction);

    void set_sampling_time(const k_time_ms sampling_time);

    float Kp() const { return _kp; }
    float Ki() const { return _ki; }
    float Kd() const { return _kd; }

    Direction direction() const { return _controller_direction; }

private:
    float _kp = 0; // * (P)roportional Tuning Parameter
    float _ki = 0; // * (I)ntegral Tuning Parameter
    float _kd = 0; // * (D)erivative Tuning Parameter

    enum Direction _controller_direction = Direction::FORWARD;

    float* _user_input = nullptr; // * Pointers to the Input, Output, and Setpoint variables
    float* _user_output = nullptr; //   This creates a hard link between the variables and the
    float* _user_setpoint = nullptr; //   PID, freeing the user from having to constantly tell us
    //   what these values are.  with pointers we'll just know.

    k_time_ms _last_time{};
    float _cumulative_output = 0;
    float _last_reading = 0;

    k_time_ms _sampling_time = k_time_ms{100};
    float _output_lower_limit = 0;
    float _output_upper_limit = 255;
    Proportionality _proportionality = Proportionality::ON_MEASUREMENT;
};

} // namespace spn::controller
