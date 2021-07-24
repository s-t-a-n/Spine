#pragma once

#include "spine/core/time.hpp"

#include <cstdarg>
#include <cstdio>

namespace spn::platform {

template<typename PlatformImp, typename PlatformConfig, typename GPIOImp, typename I2CImp, typename UARTImp>
struct Platform {
public:
    using DigitalOutput = typename GPIOImp::DigitalOutput;
    using DigitalInput = typename GPIOImp::DigitalInput;
    using AnalogueOutput = typename GPIOImp::AnalogueOutput;
    using AnalogueInput = typename GPIOImp::AnalogueInput;
    using Interrupt = typename GPIOImp::Interrupt;

public:
    using Config = PlatformConfig;

    static void initialize(Config&& cfg) { PlatformImp::initialize(std::move(cfg)); };

    static void halt(const char* msg = nullptr) { PlatformImp::halt(msg); }

    template<typename T>
    static void print(T&& msg) {
        PlatformImp::print(std::move(msg));
    };
    template<typename T>
    static void println(T&& msg) {
        PlatformImp::println(std::move(msg));
    };
    static void printflush() { PlatformImp::printflush(); }
    static void printf(const char* format, ...) {
        static char* buffer;
        constexpr auto buffer_length = 256;

        // if this function is called, a permanent chunk of memory is eaten!
        if (!buffer) {
            buffer = new char[buffer_length];
        }

        va_list args;
        va_start(args, format);
        vsnprintf(buffer, buffer_length, format, args);
        va_end(args);
        print(buffer);
    }

    // TIMING
    static time_ms millis() { return PlatformImp::millis(); }
    static time_us micros() { return PlatformImp::micros(); }

    static void delay(time_ms ms) { PlatformImp::delay_ms(time_ms(ms)); };

    static void delay_us(time_us us) { PlatformImp::delay_us(us); };
    static void delay_us(uint32_t us) { PlatformImp::delay_us(time_us(us)); };
    static void delay_ms(time_ms ms) { PlatformImp::delay_ms(ms); };
    static void delay_ms(uint32_t ms) { PlatformImp::delay_ms(time_ms(ms)); };
    // static void delay_ms(time_ms ms) { PlatformImp::delay_ms(time_ms(ms).raw()); };

    static unsigned long free_memory() { return PlatformImp::free_memory(); }

    using I2C = I2CImp;
    using UART = UARTImp;

    // SPI
    struct spi {
        using Length = typename PlatformImp::Size;
        using Duration = typename PlatformImp::Size;

        using PlatformImp::write;

        using read = typename PlatformImp::spi::read;
    };
};
} // namespace spn::platform
