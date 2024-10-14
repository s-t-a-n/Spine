#include "spine/controller/implementations/pid/pid_tuner.hpp"

namespace spn::controller {

void PIDAutotuner::start_tuning_loop(k_time_ms ms) {
    cycle = 0; // Cycle counter
    output = true; // Current output state
    outputValue = maxOutput;
    t1 = t2 = ms; // Times used for calculating period
    tHigh = tLow = k_time_ms{}; // More time variables
    max = -1000000000000; // Max input
    min = 1000000000000; // Min input
    pAverage = iAverage = dAverage = 0;
}

double PIDAutotuner::do_autotune(double input, k_time_ms ms) {
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

} // namespace spn::controller