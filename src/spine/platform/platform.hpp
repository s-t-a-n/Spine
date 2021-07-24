#pragma once

#include "spine/core/si_units.hpp"

#include <cstdarg>
#include <cstdio>

namespace spn::platform {

template<typename PlatformImp, typename PlatformConfig, typename GPIOImp, typename UARTImp>
struct Platform {
    using DigitalOutput = typename GPIOImp::DigitalOutput;
    using DigitalInput = typename GPIOImp::DigitalInput;
    using AnalogueOutput = typename GPIOImp::AnalogueOutput;
    using AnalogueInput = typename GPIOImp::AnalogueInput;
    using Interrupt = typename GPIOImp::Interrupt;
    using Config = PlatformConfig;

    /// Initialize the platform (i.e. set up monitor, etc)
    static void initialize(Config&& cfg) { PlatformImp::initialize(std::move(cfg)); };

    /// Halt the platform (disables all interrupts), optional prints a message to monitor provided by `msg`
    static void halt(const char* msg = nullptr) { PlatformImp::halt(msg); }

    template<typename T>
    /// Print to monitor (no newline)
    static void print(T&& msg) {
        PlatformImp::print(std::move(msg));
    };

    template<typename T>
    /// Print to monitor (with newline)
    static void println(T&& msg) {
        PlatformImp::println(std::move(msg));
    };

    /// Flush to monitor
    static void printflush() { PlatformImp::printflush(); }

    /// Print to monitor using the printf delivery mechanism
    static void printf(const char* format, ...) {
        constexpr auto buffer_length = 256; // completely arbitrary
        char buffer[buffer_length];

        va_list args;
        va_start(args, format);
        vsnprintf(buffer, buffer_length, format, args);
        va_end(args);
        print(buffer);
    }

    /// Returns the milliseconds expired since application start up (rolls around)
    static time_ms millis() { return PlatformImp::millis(); }

    /// Returns the microseconds expired since application start up (rolls around)
    static time_us micros() { return PlatformImp::micros(); }

    /// Sleep this thread for any provided amount of time (minimum: milliseconds)
    static void delay(time_ms ms) { PlatformImp::delay_ms(time_ms(ms)); };

    /// Sleep this thread for any provided amount of time (minimum: microseconds)
    static void delay_us(time_us us) { PlatformImp::delay_us(us); };

    /// Sleep this thread for the provided amount of time in milliseconds
    static void delay_ms(time_ms ms) { PlatformImp::delay_ms(ms); };

    /// Sleep this thread for the provided `us` microseconds
    static void delay_us(uint32_t us) { PlatformImp::delay_us(time_us(us)); };

    /// Sleep this thread for the provided `ms` in milliseconds
    static void delay_ms(uint32_t ms) { PlatformImp::delay_ms(time_ms(ms)); };

    /// Returns the amount of allocatable bytes
    static unsigned long free_memory() { return PlatformImp::free_memory(); }

    using UART = UARTImp;
};
} // namespace spn::platform
