// PID automated tuning (Ziegler-Nichols/relay method) for Arduino and compatible boards
// Copyright (c) 2016-2020 jackw01
// This code is distrubuted under the MIT License, see LICENSE for details

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

#include <algorithm>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

namespace spn::controller {
class PIDAutotuner {
public:
    // Constants for Ziegler-Nichols tuning mode
    enum class Aggressiveness { BasicPID, LessOvershoot, NoOvershoot };

    PIDAutotuner() {}

    // Configure parameters for PID tuning
    // See README for more details - https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
    // targetInputValue: the target value to tune to
    // loopInterval: PID loop interval in milliseconds - must match whatever the PID loop being tuned runs at
    // outputRange: min and max values of the output that can be used to control the system (0, 255 for analogWrite)
    // znMode: Ziegler-Nichols tuning mode (znModeBasicPID, znModeLessOvershoot, znModeNoOvershoot)
    // tuningCycles: number of cycles that the tuning runs for (optional, default is 10)
    void set_target_input_value(double target) { targetInputValue = target; }
    void set_loop_interval(time_ms interval) { loopInterval = interval; }

    // Set hysteresis; the minimal overshoot over `targetInputValue`
    // This is to discourage a sensor bouncing below and over the `targetInputValue` causing a cycle with a false length
    void set_hysteresis(double hysteresis) { targetHysteresis = std::fabs(hysteresis); }

    // Set output range
    void setOutputRange(double lower_limit, double upper_limit) {
        minOutput = lower_limit;
        maxOutput = upper_limit;
    }
    // Set Ziegler-Nichols tuning mode
    void set_aggressiveness(Aggressiveness zn) { znMode = zn; }
    // Set tuning cycles
    void set_tuning_cycles(int tuneCycles) { cycles = tuneCycles; }

    // Must be called immediately before the tuning loop starts
    void start_tuning_loop(time_ms ms) {
        cycle = 0; // Cycle counter
        output = true; // Current output state
        outputValue = maxOutput;
        t1 = t2 = ms; // Times used for calculating period
        tHigh = tLow = time_ms{}; // More time variables
        max = -1000000000000; // Max input
        min = 1000000000000; // Min input
        pAverage = iAverage = dAverage = 0;
    }

    // Automatically tune PID
    // This function must be run in a loop at the same speed as the PID loop being tuned
    // See README for more details - https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
    double do_autotune(double input, time_ms ms) {
        // Useful information on the algorithm used (Ziegler-Nichols method/Relay method)
        // http://www.processcontrolstuff.net/wp-content/uploads/2015/02/relay_autot-2.pdf
        // https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method
        // https://www.cds.caltech.edu/~murray/courses/cds101/fa04/caltech/am04_ch8-3nov04.pdf

        // Basic explanation of how this works:
        //  * Turn on the output of the PID controller to full power
        //  * Wait for the output of the system being tuned to reach the target input value
        //      and then turn the controller output off
        //  * Wait for the output of the system being tuned to decrease below the target input
        //      value and turn the controller output back on
        //  * Do this a lot
        //  * Calculate the ultimate gain using the amplitude of the controller output and
        //      system output
        //  * Use this and the period of oscillation to calculate PID gains using the
        //      Ziegler-Nichols method

        // Calculate max and min
        max = (max > input) ? max : input;
        min = (min < input) ? min : input;

        // Output is on and input signal has risen to target
        if (output && input > targetInputValue + targetHysteresis) {
            // Turn output off, record current time as t1, calculate tHigh, and reset maximum
            output = false;
            outputValue = minOutput;
            t1 = ms;
            tHigh = t1 - t2;
            max = targetInputValue;
        }

        // Output is off and input signal has dropped to target
        if (!output && input < targetInputValue - targetHysteresis) {
            // Turn output on, record current time as t2, calculate tLow
            output = true;
            outputValue = maxOutput;
            t2 = ms;
            tLow = t2 - t1;

            // Calculate Ku (ultimate gain)
            // Formula given is Ku = 4d / πa
            // d is the amplitude of the output signal
            // a is the amplitude of the input signal
            const double ku = (4.0 * ((maxOutput - minOutput) / 2.0)) / (M_PI * (max - min) / 2.0);

            // Calculate Tu (period of output oscillations)
            const double tu = tLow.raw() + tHigh.raw();

            // How gains are calculated
            // PID control algorithm needs Kp, Ki, and Kd
            // Ziegler-Nichols tuning method gives Kp, Ti, and Td
            //
            // Kp = 0.6Ku = Kc
            // Ti = 0.5Tu = Kc/Ki
            // Td = 0.125Tu = Kd/Kc
            //
            // Solving these equations for Kp, Ki, and Kd gives this:
            //
            // Kp = 0.6Ku
            // Ki = Kp / (0.5Tu)
            // Kd = 0.125 * Kp * Tu

            // Constants
            // https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method

            double kpConstant, tiConstant, tdConstant;
            if (znMode == Aggressiveness::BasicPID) {
                kpConstant = 0.6;
                tiConstant = 0.5;
                tdConstant = 0.125;
            } else if (znMode == Aggressiveness::LessOvershoot) {
                kpConstant = 0.33;
                tiConstant = 0.5;
                tdConstant = 0.33;
            } else { // Default to No Overshoot mode as it is the safest
                kpConstant = 0.2;
                tiConstant = 0.5;
                tdConstant = 0.33;
            }

            // Calculate gains
            kp = kpConstant * ku;
            ki = (kp / (tiConstant * tu)) * loopInterval.raw();
            kd = (tdConstant * kp * tu) / loopInterval.raw();

            // Average all gains after the first two cycles
            if (cycle > 1) {
                pAverage += kp;
                iAverage += ki;
                dAverage += kd;
            }

            // Reset minimum
            min = targetInputValue;

            // Increment cycle count
            cycle++;
        }

        // If loop is done, disable output and calculate averages
        if (is_finished()) {
            output = false;
            outputValue = minOutput;
            kp = pAverage / (cycle - 1);
            ki = iAverage / (cycle - 1);
            kd = dAverage / (cycle - 1);
        }

        return outputValue;
    }

    // Get results of most recent tuning
    double get_Kp() { return kp; };
    double get_Ki() { return ki; };
    double get_Kd() { return kd; };

    // Is the tuning loop finished?
    bool is_finished() { return (cycle >= cycles); }

    // return number of tuning cycle
    int get_cycle() { return cycle; }

private:
private:
    double targetInputValue = 0;
    double targetHysteresis = 0;
    time_ms loopInterval;
    double minOutput, maxOutput;
    Aggressiveness znMode = Aggressiveness::NoOvershoot;
    int cycles = 10;

    int cycle; // current cycle
    bool output; // is output on
    double outputValue; // current output value
    time_ms t1, t2, tHigh, tLow;
    double max, min;
    double pAverage, iAverage, dAverage;
    double kp, ki, kd;
};

} // namespace spn::controller
