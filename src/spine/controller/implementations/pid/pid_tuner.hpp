/*********************************************************************************************
 * PID automated tuning (Ziegler-Nichols/relay method) for Arduino and compatible boards
 * Copyright (c) 2016-2020 jackw01
 *
 * This library was originally created by jackw01 and is licensed under MIT.
 *
 * Downloaded on 14-05-2024 from https://github.com/jackw01/arduino-pid-autotuner
 *
 * Modifications by Stan:
 * - naming made consistent within Spine
 * - use Spine's time units
 * - added hysteresis to overcome bouncing
 * - remove unnecessary functionality
 **********************************************************************************************/

#pragma once

#include "spine/structure/units/si.hpp"

#include <algorithm>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

namespace spn::controller {
class PIDAutotuner {
public:
    // Constants for Ziegler-Nichols tuning mode
    enum class Aggressiveness { BasicPID, LessOvershoot, NoOvershoot };

    PIDAutotuner() = default;

    /// Configure parameters for PID tuning
    /// See README for more details - https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
    /// targetInputValue: the target value to tune to
    /// loopInterval: PID loop interval in milliseconds - must match whatever the PID loop being tuned runs at
    /// outputRange: min and max values of the output that can be used to control the system (0, 255 for analogWrite)
    /// znMode: Ziegler-Nichols tuning mode (znModeBasicPID, znModeLessOvershoot, znModeNoOvershoot)
    /// tuningCycles: number of cycles that the tuning runs for (optional, default is 10)
    void set_target_input_value(float target) { targetInputValue = target; }
    void set_loop_interval(k_time_ms interval) { loopInterval = interval; }

    /// Set hysteresis; the minimal overshoot over `targetInputValue`
    /// This is to discourage a sensor bouncing below and over the `targetInputValue` causing a cycle with a false
    /// length
    void set_hysteresis(float hysteresis) { targetHysteresis = std::fabs(hysteresis); }

    /// Set output range
    void setOutputRange(float lower_limit, float upper_limit) {
        minOutput = lower_limit;
        maxOutput = upper_limit;
    }
    /// Set Ziegler-Nichols tuning mode
    void set_aggressiveness(Aggressiveness zn) { znMode = zn; }
    /// Set tuning cycles
    void set_tuning_cycles(int tuneCycles) { cycles = tuneCycles; }

    /// Must be called immediately before the tuning loop starts
    void start_tuning_loop(k_time_ms ms);

    /// Automatically tune PID
    /// This function must be run in a loop at the same speed as the PID loop being tuned
    /// See README for more details - https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
    float do_autotune(float input, k_time_ms ms);

    /// Get results of most recent tuning
    float get_Kp() const { return kp; };
    float get_Ki() const { return ki; };
    float get_Kd() const { return kd; };

    /// Is the tuning loop finished?
    bool is_finished() const { return (cycle >= cycles); }

    /// return number of tuning cycle
    int get_cycle() const { return cycle; }

private:
private:
    float targetInputValue = 0;
    float targetHysteresis = 0;
    k_time_ms loopInterval = {};
    float minOutput = 0, maxOutput = 0;
    Aggressiveness znMode = Aggressiveness::NoOvershoot;
    int cycles = 10;

    int cycle = 0; // current cycle
    bool output = false; // is output on
    float outputValue = 0; // current output value
    k_time_ms t1 = {}, t2 = {}, tHigh = {}, tLow = {};
    float max = 0, min = 0;
    float pAverage = 0, iAverage = 0, dAverage = 0;
    float kp = 0, ki = 0, kd = 0;
};

} // namespace spn::controller
