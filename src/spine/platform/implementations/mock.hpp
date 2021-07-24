#pragma once

#ifdef NATIVE

#    include "spine/core/debugging.hpp"
#    include "spine/io/stream/stream.hpp"
#    include "spine/platform/gpio.hpp"
#    include "spine/platform/platform.hpp"
#    include "spine/platform/protocols/i2c.hpp"
#    include "spine/platform/protocols/uart.hpp"

#    include <algorithm>
#    include <cmath>
#    include <cstddef>
#    include <cstdint>
#    include <cstdlib>
#    include <cstring>
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
    void set_state_impl(LogicalState state) {}

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

    void initialize() { set_value(0.0); }

    // value between 0 and 1 where 1 is the logical state ON
    void set_value(double value) {
        assert(value >= 0.0 && value <= 1.0);
        if (_cfg.active_on_low) value = 1.0 - value;
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
    double read() const { return 0; }

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

/// not implemented
class MockI2C : public I2C<MockI2C> {
public:
    struct Config {};

    MockI2C(Config&& cfg) : _cfg(std::move(cfg)) {}

    static void initialize() {}
    static uint16_t available() { return 0; };
    static uint16_t read(uint8_t* buffer, uint16_t length) { return 0; }
    static uint8_t read() { return 0; }
    static uint16_t write(uint8_t* buffer, uint16_t length, uint8_t address) { return 0; }

private:
    const Config _cfg;
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
    using AnalogueOutput = MockAnalogueOutput;
    using AnalogueInput = MockAnalogueInput;

    using Interrupt = MockInterrupt;
};

struct MockConfig {
    uint32_t baudrate;
};

// since no situation is feasible where more than one `Platform` exist at the same time and this is lazy mockery
// this can safely be a global singleton. obviously thread-unsafe.
static inline struct MockState {
    time_t millis = 0;
    time_t micros = 0;
} g_mock_state;

struct Mock : Platform<Mock, MockConfig, MockGPIO, MockI2C, MockUART> {
    static void initialize(Config&& cfg){};

    static void halt(const char* msg = nullptr) {}

    template<typename T>
    static void print(T& msg){};
    template<typename T>
    static void println(T& msg){};
    template<typename T>
    static void print(T&& msg){};
    template<typename T>
    static void println(T&& msg){};

    static void printflush() {}

    // TIMING
    static time_ms millis() { return time_ms(g_mock_state.millis + g_mock_state.micros / 1000); }
    static time_us micros() { return time_us(g_mock_state.micros + g_mock_state.millis * 1000); }
    static void delay_us(time_us us) { g_mock_state.micros += us.raw(); }
    static void delay_ms(time_ms ms) { g_mock_state.millis += ms.raw(); }

    static unsigned long free_memory() { return 0; };
};

} // namespace spn::platform

#endif
