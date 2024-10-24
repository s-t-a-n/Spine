#pragma once

#ifdef NATIVE

#    define SPINE_PLATFORM_CAP_UART
#    define SPINE_PLATFORM_CAP_GPIO
#    define SPINE_PLATFORM_CAP_GPIO_INTERRUPTS
#    define SPINE_PLATFORM_CAP_ADC
#    define SPINE_PLATFORM_CAP_PWM
#    define SPINE_PLATFORM_CAP_PRINT
#    define SPINE_PLATFORM_CAP_PRINTF
#    define SPINE_PLATFORM_CAP_MEMORY_METRICS

#    include "spine/core/debugging.hpp"
#    include "spine/io/stream/stream.hpp"
#    include "spine/platform/gpio.hpp"
#    include "spine/platform/platform.hpp"
#    include "spine/platform/protocols/uart.hpp"

#    include <algorithm>
#    include <cmath>
#    include <cstddef>
#    include <cstdint>
#    include <cstdlib>
#    include <cstring>
#    include <iostream>
#    include <limits>
#    include <memory>
#    include <type_traits>
#    include <utility>

// todo : fill in functionality and write unittests for it

namespace spn::platform {

/// not implemented
class MockDigitalOutput : public DigitalOutput<MockDigitalOutput> {
public:
    struct Config {
        uint8_t pin;
        bool active_on_low;
    };

    MockDigitalOutput(const Config&& cfg) : _cfg(std::move(cfg)) {}

protected:
    void initialize_impl() {}
    void set_state_impl(core::LogicalState state) {}

    friend DigitalOutput;

private:
    const Config _cfg;
};

/// not implemented
class MockDigitalInput : public DigitalInput<MockDigitalInput> {
public:
    struct Config {
        uint8_t pin;
        bool pull_up = false;
    };

    MockDigitalInput(const Config&& cfg) : _cfg(std::move(cfg)) {}

    void initialize() {}

    [[nodiscard]] bool state() const { return false; }

private:
    const Config _cfg;
};

/// not implemented
class MockAnalogueOutput : public AnalogueOutput<MockAnalogueOutput> {
public:
    struct Config {
        uint8_t pin;
        bool active_on_low;
        uint8_t resolution = 8;
    };

    MockAnalogueOutput(const Config&& cfg) : _cfg(std::move(cfg)) {}

    ~MockAnalogueOutput() = default;

    void initialize() { set_value(0.0f); }

    // value between 0 and 1 where 1 is the logical state ON
    void set_value(float value) {
        spn_assert(value >= 0.0f && value <= 1.0f);
        if (_cfg.active_on_low) value = 1.0f - value;
    }

private:
    const Config _cfg;
};

/// not implemented
class MockAnalogueInput : public AnalogueInput<MockAnalogueInput> {
public:
    struct Config {
        uint8_t pin;
        bool pull_up;
    };

    MockAnalogueInput(const Config&& cfg) : _cfg(std::move(cfg)) {}

    void initialize() {}
    float read() const { return 0; }

private:
    const Config _cfg;
};

/// not implemented
class MockInterrupt : public Interrupt<MockInterrupt> {
public:
    struct Config {
        uint8_t pin;
        TriggerType mode;
        bool pull_up;
        void (*callback)();
    };

    MockInterrupt(Config&& cfg)
        : _cfg(std::move(cfg)), //
          _mode(cfg.mode), //
          _callback(cfg.callback) {}

    void initialize() {}

    void attach_interrupt(void (*callback)() = nullptr, TriggerType trigger = TriggerType::UNDEFINED) {}
    void detach_interrupt() {}

private:
    const Config _cfg;

    TriggerType _mode;
    void (*_callback)();
};

class Print {
public:
    virtual size_t write(uint8_t) = 0;
    virtual size_t write(const uint8_t* buffer, size_t size) = 0;
};

class MockUART : public UART<MockUART> {
public:
    struct Config {
        io::Stream* stream;
    };

    MockUART(Config&& cfg) : _cfg(std::move(cfg)), _stream_ref(_cfg.stream) {}

    void initialize() override { return _stream_ref->initialize(); };
    size_t read(uint8_t* const buffer, size_t length) override { return _stream_ref->read(buffer, length); }
    bool read(uint8_t& value) override { return _stream_ref->read(value); }
    size_t write(const uint8_t* const buffer, size_t length) override { return _stream_ref->write(buffer, length); }
    bool write(uint8_t value) override { return _stream_ref->write(value); }
    size_t available() const override { return _stream_ref->available(); }
    size_t available_for_write() const override { return _stream_ref->available_for_write(); }
    void flush() override { _stream_ref->flush(); }

private:
    const Config _cfg;

    io::Stream* _stream_ref;
};

struct MockGPIO {
    using DigitalOutput = MockDigitalOutput;
    using DigitalInput = MockDigitalInput;
    using Interrupt = MockInterrupt;
};

struct MockAnalogue {
    using AnalogueOutput = MockAnalogueOutput;
    using AnalogueInput = MockAnalogueInput;
};

struct MockConfig {
    uint32_t baudrate;
};

// since no situation is feasible where more than one `Platform` exist at the same time and this is lazy mockery
// this can safely be a global singleton. obviously thread-unsafe.
struct MockState {
    k_time_ms millis = k_time_ms(0);
    k_time_us micros = k_time_us(0);
};

MockState& MockStateInstance();

struct Mock : Platform<Mock, MockConfig, MockGPIO, MockAnalogue, MockUART> {
    static void initialize(Config&& cfg){};

    static void halt(const char* msg = nullptr) {}

    template<typename T>
    static void print(T& msg) {
        std::cout << msg;
    };
    template<typename T>
    static void println(T& msg) {
        std::cout << msg << std::endl;
    };
    template<typename T>
    static void print(T&& msg) {
        std::cout << msg;
    };
    template<typename T>
    static void println(T&& msg) {
        std::cout << msg << std::endl;
    };

    static void printflush() {}

    static k_time_ms millis() { return k_time_ms(MockStateInstance().millis + MockStateInstance().micros / 1000); }
    static k_time_us micros() { return k_time_us(MockStateInstance().micros + MockStateInstance().millis * 1000); }
    static void delay_us(k_time_us us) { MockStateInstance().micros += us; }
    static void delay_ms(k_time_ms ms) { MockStateInstance().millis += ms; }

    static unsigned long free_memory() { return 0; };
};

} // namespace spn::platform

#endif
