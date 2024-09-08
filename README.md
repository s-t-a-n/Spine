# Spine

![ci](https://github.com/s-t-a-n/Spine/actions/workflows/ci.yml/badge.svg)
[![last commit](https://badgen.net/github/last-commit/s-t-a-n/KasKas)](https://GitHub.com/s-t-a-n/Spine/commit/)
[![license](https://img.shields.io/github/license/s-t-a-n/KasKas.svg)](https://github.com/s-t-a-n/Spine/blob/main/LICENSE)

Spine is a reusable components library aimed at robotics, providing a range of core functionalities such as signal
processing, an eventsystem, logging, data streaming, and hardware abstraction layers for Arduino, STM32, and mock
environments. It also includes various data structures and controllers that are embedded friendly.

Aimed at RISC-like platforms. I have choosen to drop 8/16 bit AVR as their compilers have aged so far that c++14 is
barely supported, let alone critical STL stuff like array, shared_ptr or optional. This is why you can find an array and
vector implementation, even though in FilterStack, an STL's vector implementation is used.

## Design philosophy

- reusability and readability over raw speed
- single allocation to combat our ultimate nemesis; fragmentation
- not to be extensive, but to be simple to use
- nothing should be added that is not immediately needed
- performance by hardware first; choose to sacrifice speed/space when doing so increases maintainability. For example:
  be generous with debugging statements (which take RAM/flash space) and extend flash rather than using obscure
  error codes

## Core features

- Hardware Abstraction Layer: HAL aimed at removing platform specified code. Theoretically allowing a seemless jump from
  for example Arduino, to STM32, to ESP32, to mock as all platforms are essentially doing the same thing.

### Contents

- **Controllers**: Includes PID controller, SetReset-Latch.
- **Core**: Provides si-units and datetime utilities, debugging tools, exception handling, and scheduling.
- **Event System**: Manages events and pipelines.
- **Filters**: Offers various filter implementations rolled into a filterstack including simple bandpass, EWMA,
  inverting, transpositiong and passthrough.
- **I/O**: Contains generic sensor interface and various streams with support for buffered streams and UART '
  transactions'.
- **Platform**: Abstraction layers for GPIO, HAL, bus-protocols (I2C, UART). Currently only Arduino and native X86 are
  supported.
- **Structure**: Various data structures such as arrays, ring buffers, stacks, memorypools, static string and vectors
  that adhere to single allocation principle to eliminate fragmentation.
- **Tracker**: Tools for monitoring average changes and runaway conditions.

## How to run

1. Get PlatformIO.
2. Run `pio run` in the root of repository to compile

## How to test

1. Get PlatformIO.
2. Run `pio test -e unittest` in the root of repository to compile

## Examples

### PIDController

#### Basic operation

```
#include "controller/pid/pid_controller.hpp"

int main() {
    spine::controller::PIDController pid(1.0, 0.1, 0.01);
    double setpoint = 100;
    double measured_value = 90;
    double control_value = pid.compute(setpoint, measured_value);
    std::cout << "Control Value: " << control_value << std::endl;
    return 0;
}
```

#### Autotune

```
int main() {
    spine::controller::PIDController pid(1.0, 0.1, 0.01);

    const auto autotune_setpoint = 50.0;

    // activate peripherals here

    const auto process_getter = [&]() { /* return _sensor_.value();*/ };
    const auto process_setter = [&](double value) { /* _actuator.set_to(value); */ };
    const auto process_loop = [&]() { /* _hws.update_all(); */ };

    pid.autotune(
        PID::TuneConfig{.setpoint = autotune_setpoint,
                        .startpoint = 0, /* the point to which the process will be satured before tuning */
                        .hysteresis = 0.3, /* the sensor hysteresis */
                        .satured_at_start = false, /* i.e. does the process need to be satured before tuning? */
                        .cycles = 30,
                        .aggressiveness = spn::controller::PIDAutotuner::ZNMode::BasicPID},
        process_setter,
        process_getter,
        process_loop);
    
    // as of writing, the output will be displayed on the UART output.
    // todo: return a triplet containing the tuning parameters

    // deactivate peripherals here
}
```

