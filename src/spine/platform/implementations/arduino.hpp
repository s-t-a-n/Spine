#pragma once

#define SPINE_PLATFORM_CAP_UART
#define SPINE_PLATFORM_CAP_GPIO
#define SPINE_PLATFORM_CAP_GPIO_INTERRUPTS
#define SPINE_PLATFORM_CAP_ADC
#define SPINE_PLATFORM_CAP_PWM
#define SPINE_PLATFORM_CAP_PRINT
#define SPINE_PLATFORM_CAP_PRINTF
#define SPINE_PLATFORM_CAP_MEMORY_METRICS

#include "spine/core/debugging.hpp"
#include "spine/core/logging.hpp"
#include "spine/platform/gpio.hpp"
#include "spine/platform/implementations/arduino_without_sugar.hpp"
#include "spine/platform/platform.hpp"
#include "spine/platform/protocols/uart.hpp"

#include <Wire.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

// Platform specific includes
#ifdef ARDUINO_ARCH_ESP32
#    include "driver/uart.h" // ESP32-specific include to manipulate the RX/TX buffer.
#endif

namespace spn::platform {

class ArduinoDigitalOutput : public DigitalOutput<ArduinoDigitalOutput> {
public:
    struct Config {
        uint8_t pin;
        bool active_on_low;
        // Config(uint8_t pin, bool active_on_low = false) : pin(pin), active_on_low(active_on_low){};
    };

    ArduinoDigitalOutput(const Config&& cfg) : _cfg(std::move(cfg)) {}

protected:
    void initialize_impl() { pinMode(_cfg.pin, OUTPUT); }

    void set_state_impl(core::LogicalState state) {
        if (state == core::LogicalState::ON) digitalWrite(_cfg.pin, !_cfg.active_on_low);
        else
            digitalWrite(_cfg.pin, _cfg.active_on_low);
    }

    friend DigitalOutput;

private:
    const Config _cfg;
};

class ArduinoDigitalInput : public DigitalInput<ArduinoDigitalInput> {
public:
    struct Config {
        uint8_t pin;
        bool pull_up = false;

        // Config(uint8_t pin, bool pull_up = false) : pin(pin), pull_up(pull_up){};
    };

    ArduinoDigitalInput(const Config&& cfg) : _cfg(std::move(cfg)) {}

    void initialize() {
        pinMode(_cfg.pin, INPUT);
        if (_cfg.pull_up) pinMode(_cfg.pin, INPUT_PULLUP);
    }

    [[nodiscard]] bool state() const { return digitalRead(_cfg.pin); }

private:
    const Config _cfg;
};

class ArduinoAnalogueOutput : public AnalogueOutput<ArduinoAnalogueOutput> {
public:
    struct Config {
        uint8_t pin;
        bool active_on_low;
        uint8_t resolution = 8;
    };

    ArduinoAnalogueOutput(const Config&& cfg) : _cfg(std::move(cfg)) {}

    ~ArduinoAnalogueOutput() {}

    void initialize() {
        analogWriteResolution(_cfg.resolution);
        set_value(0.0);
    }

    // value between 0 and 1 where 1 is the logical state ON
    void set_value(double value) {
        spn_assert(value >= 0.0 && value <= 1.0);
        if (_cfg.active_on_low) value = 1.0 - value;

        analogWrite(_cfg.pin, int(value * std::pow(2, _cfg.resolution)));
    }

private:
    const Config _cfg;
};

class ArduinoAnalogueInput : public AnalogueInput<ArduinoAnalogueInput> {
public:
    struct Config {
        uint8_t pin;
        bool pull_up;
    };

    ArduinoAnalogueInput(const Config&& cfg) : _cfg(std::move(cfg)) {}

    void initialize() {
        if (_cfg.pull_up) pinMode(_cfg.pin, INPUT_PULLUP);
    }

    //     void set_reference_voltage(double voltage, bool external = false) {
    //         // https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
    // #    if defined(__AVR_ATmega8__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
    //         constexpr auto v330 = DEFAULT;
    // #    elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    //         constexpr auto v330 = DEFAULT;
    // #    elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega1284P__) \
//         || defined(__AVR_ATmega1284__)
    //         constexpr auto v330 = DEFAULT;
    // #    elif defined(ARDUINO_SAM_DUE)
    //         constexpr auto v330 = AR_DEFAULT;
    // #    else
    //         constexpr auto v330 = AR_DEFAULT;
    // #    endif
    //
    //         switch (static_cast<int>(voltage) * 100) {
    //         case 110: break;
    //         case 330: analogReference(v330); break;
    //         case 500: break;
    //         }
    //     }

    double read() const {
        // todo: noise suppression?
        //  const auto sample_count = 50;
        //  double sample_cum = 0;
        //  for (int i = 0; i < sample_count; ++i) {
        //      sample_cum += analogRead(_cfg.pin);
        //      delay(1);
        //  }
        //  double sample = sample_cum / sample_count;
        return analogRead(_cfg.pin) / 1024.0; // assume 10-bit resolution
    }

private:
    const Config _cfg;
};

class ArduinoInterrupt : public Interrupt<ArduinoInterrupt> {
public:
    struct Config {
        uint8_t pin;
        TriggerType mode;
        bool pull_up;
        void (*callback)();

        // Config(uint8_t pin, Mode mode = Mode::NOP, bool pull_up = false, void (*callback)() = nullptr)
        // : pin(pin), mode(mode), pull_up(pull_up), callback(callback){};
    };

    ArduinoInterrupt(Config&& cfg)
        : _cfg(std::move(cfg)), //
          _mode(cfg.mode), //
          _callback(cfg.callback) {}

    void initialize() {
        pinMode(_cfg.pin, INPUT);
        if (_cfg.pull_up) pinMode(_cfg.pin, INPUT_PULLUP);
    }

    void attach_interrupt(void (*callback)() = nullptr, TriggerType trigger = TriggerType::UNDEFINED) {
        int mode_bits;
        auto callback_actual = callback ? callback : _callback;
        //        spn_assert(callback_actual);

        auto mode_actual = trigger != TriggerType::UNDEFINED ? trigger : _mode;
        //        spn_assert(mode_actual != Mode::NOP);

        switch (mode_actual) {
        case TriggerType::RISING_AND_FALLING_EDGE: mode_bits = 1; break;
        case TriggerType::FALLING_EDGE: mode_bits = 2; break;
        case TriggerType::RISING_EDGE: mode_bits = 3; break;
        default: mode_bits = 0; break;
        }

        _mode = mode_actual;
        _callback = callback_actual;

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
        ::attachInterrupt(_cfg.pin, callback_actual, mode_bits);
#else
        spn_assert(digitalPinToInterrupt(_cfg.pin) != NOT_AN_INTERRUPT);
        ::attachInterrupt(digitalPinToInterrupt(_cfg.pin), callback_actual, mode_bits);
#endif
    }

    void detach_interrupt() { ::detachInterrupt(_cfg.pin); }

private:
    const Config _cfg;

    TriggerType _mode;
    void (*_callback)();
};

// the minds of the Arduino team for some obscure reason have left an incomplete 'HardwareSerial' stream
// that doesnt implement 'availableForWrite'. We are left to wonder why a UART/USART class would inherit from a Serial
// class and not the other way around (Interface>Implementation). Since SAM is the only platform from Arduino's own
// lineup that is supported, we make a special case for this board.
#if defined(SAM)
#    define UART_T ::UARTClass
#else
#    define UART_T ::HardwareSerial
#endif

class ArduinoUART : public UART<ArduinoUART> {
public:
    struct Config {
        ::Stream* stream; // Arduino HardwareSerial stream, typically 'Serial'
        time_ms timeout = time_ms(1000); //
    };

    ArduinoUART(Config&& cfg) : _cfg(std::move(cfg)), _stream_ref(static_cast<UART_T*>(_cfg.stream)) {}

    void initialize() override { _stream_ref->setTimeout(_cfg.timeout.raw<unsigned long>()); }

    size_t available() const override {
        spn_assert(_stream_ref);
        return static_cast<size_t>(_stream_ref->available());
    };
    size_t available_for_write() const override {
        spn_assert(_stream_ref);
        return static_cast<size_t>(_stream_ref->availableForWrite());
    }

    void flush() override { _stream_ref->flush(); }

    size_t read(uint8_t* const buffer, size_t length) override { return _stream_ref->readBytes(buffer, length); }
    bool read(uint8_t& value) override {
        value = _stream_ref->read();
        return true;
    }

    size_t write(const uint8_t* const buffer, size_t length) override {
        const auto bytes_written = _stream_ref->write(buffer, length);
        return bytes_written;
    }
    bool write(const uint8_t value) override { return _stream_ref->write(value); }

private:
    const Config _cfg;

    UART_T* _stream_ref;
};

struct ArduinoGPIO {
    using DigitalOutput = ArduinoDigitalOutput;
    using DigitalInput = ArduinoDigitalInput;
    using Interrupt = ArduinoInterrupt;
};

struct ArduinoAnalogue {
    using AnalogueOutput = ArduinoAnalogueOutput;
    using AnalogueInput = ArduinoAnalogueInput;
};

struct ArduinoConfig {
    uint32_t baudrate = 115200;
};

struct Arduino : public Platform<Arduino, ArduinoConfig, ArduinoGPIO, ArduinoAnalogue, ArduinoUART> {
    static void initialize(Config&& cfg) {
#if defined(NATIVE)
        ARDUINO_MOCK_ALL();
#endif
        if (cfg.baudrate > 0) {
            Serial.begin(cfg.baudrate);

#ifdef ARDUINO_ARCH_ESP32
            uart_driver_delete(UART_NUM_0); // Reinstall the UART driver with custom RX and TX buffer sizes
            uart_driver_install(UART_NUM_0, SERIAL_RX_BUFFER_SIZE, SERIAL_TX_BUFFER_SIZE, 0, NULL, 0);
#endif // Both AVR's and STM's rolls of Arduino respect the SERIAL_RX/TX_BUFFER_SIZE macros
        }

        // todo: set this up separately in an ADC/DAC module
        // analogReadResolution(10); // 0-1023
        // analogWriteResolution(8); // 0-255
    };

    static void halt(const char* msg = NULL) {
        SPN_LOG("Halting with reason: %s", msg ? msg : "[not specified]");
        delay(time_s(2));
        noInterrupts();
        while (true) {
        };
    }

    template<typename T>
    static void print(T& msg) {
        Serial.print(msg);
    };
    template<typename T>
    static void println(T& msg) {
        Serial.println(msg);
    };
    template<typename T>
    static void print(T&& msg) {
        Serial.print(std::move(msg));
    };
    template<typename T>
    static void println(T&& msg) {
        Serial.println(std::move(msg));
    };

    static void printflush() { Serial.flush(); }

    static time_ms millis() { return time_ms(::millis()); }
    static time_us micros() { return time_us(::micros()); }
    static void delay_us(time_us us) { ::delayMicroseconds(us.raw<uint32_t>()); }
    static void delay_ms(time_ms ms) { ::delay(ms.raw<uint32_t>()); }

    static unsigned long free_memory();
};

} // namespace spn::platform
