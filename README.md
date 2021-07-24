# Spine

[![ci](https://github.com/s-t-a-n/Spine/actions/workflows/ci.yml/badge.svg)](https://github.com/s-t-a-n/Spine/actions?query=workflow=ci)
[![last commit](https://badgen.net/github/last-commit/s-t-a-n/Spine)](https://GitHub.com/s-t-a-n/Spine/commit/)
[![license](https://img.shields.io/github/license/s-t-a-n/Spine.svg)](https://github.com/s-t-a-n/Spine/blob/main/LICENSE)

Spine is a reusable components library aimed at low frequency data robotics/IoT, providing a range of core
functionalities
such as signal
processing, event handling, logging, data streaming, and hardware abstraction layers for Arduino, STM32, and mock
environments. It also includes various data structures and controllers that are embedded friendly. I have gobbled
together stuff over the years and would like to build this out in a more directed effort.

Aimed at 32bit RISC-platforms that have a dedicated FPU like the Cortex M4.

## Design philosophy

- opposite of bare metal: instead of using platform specific registers/peripherals/features emulate as much as possible
  on top of platform specifics such as to make a most broadly applicable codebase; for as much as is feasible let
  hardware decide the how and let software decice the what
- reusability and readability over raw speed
- single allocation to combat our ultimate nemesis; fragmentation
- not to be extensive, but to be simple to use
- nothing should be added that is not immediately needed
- performance by hardware first; choose to sacrifice speed/space when doing so increases maintainability. For example:
  be generous with debugging statements even though they ravenously eat flash and rather extend the flash than using
  obscure error codes everywhere

## Core features

- Abstract Hardware Abstraction Layer: Where a run-of-the-mill vendor's HAL aims to remove the focus from hardware
  specifics, this HAL tries to
  top that by making any hardware request a conceptual act. When absolute peak performance is not a requirement, why
  would one care to use 'GPIOA->ODR |= GPIO_ODR_OD5;' (Cortex-M), or through a HAL with digitalWrite (Arduino),
  HAL_GPIO_WritePin (STM32), "GPIO.output(18, GPIO.HIGH)" (RPI), etc. when conceptually one intends to say "i want that
  pin to be logically on"? The implementation found under src/spine/platform theoretically allows a seemless jump
  between various Hardware/HAL implementations, effectively _attempting_ to sever software from hardware. As civil
  engineers say: One more lane will fix it!

### Contents

- **Platform**: Abstraction layers for GPIO, HAL, bus-protocols (I2C, UART). Currently only Arduino and native X86 are
  supported.
- **Controllers**: Includes PID controller, SetReset-Latch.
- **Core**: Provides si-units and datetime utilities, debugging tools, exception handling, and scheduling.
- **Event System**: Manages events and pipelines.
- **Filters**: Offers various filter implementations rolled into a filterstack including simple bandpass, EWMA,
  inverting, transpositiong and passthrough.
- **I/O**: Contains generic sensor interface and various streams with support for buffered streams and UART '
  transactions'.
- **Structure**: Various data structures such as arrays, ring buffers, stacks, memorypools, static string and vectors
  that adhere to single allocation principle to eliminate fragmentation.
- **Tracker**: Tools for monitoring average changes and runaway conditions.

## How to run

1. Get PlatformIO.
2. Run `pio run` in the root of repository to compile for every target a sample main file.
3. Run `pio test -e unittest` in the root of repository to run the unittests

## How to use

Use the example like any other library.

## Examples

See the `examples` or the `tests` folder.

## How to contribute

Rip everything you come across that you like. If you improve it, be a good lad and throw back a PR!

## References

- [Hexagonal architecture](https://alistair.cockburn.us/hexagonal-architecture/)
- [Modern C++ White paper:
  Making things do stuff](https://www.feabhas.com/sites/default/files/uploads/EmbeddedWisdom/Feabhas%20Modern%20C%2B%2B%20white%20paper%20Making%20things%20do%20stuff.pdf)
