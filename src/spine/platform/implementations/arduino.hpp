#pragma once

#ifdef ARDUINO

#    if not defined(UNITTEST)
#        include "spine/platform/implementations/arduino_without_sugar.hpp"
#    else
#        include <ArduinoFake.h>
#    endif
#    include "spine/core/debugging.hpp"
#    include "spine/platform/gpio.hpp"
#    include "spine/platform/platform.hpp"
#    include "spine/platform/protocols/i2c.hpp"
#    include "spine/platform/protocols/uart.hpp"

#    include <ArxSmartPtr.h>
#    include <Wire.h>

#    include <algorithm>
#    include <cmath>
#    include <cstddef>
#    include <cstdint>
#    include <cstdlib>
#    include <limits>
#    include <memory>
#    include <type_traits>
#    include <utility>

#    if defined(NATIVE)

#        define ARDUINO_MOCK_ALL()                                                                                     \
            {                                                                                                          \
                using namespace fakeit;                                                                                \
                When(Method(ArduinoFake(), pinMode)).Return();                                                         \
                When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();                                              \
                When(Method(ArduinoFake(), digitalRead)).AlwaysReturn(0);                                              \
                When(Method(ArduinoFake(), analogRead)).AlwaysReturn(0);                                               \
                When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();                                               \
                When(Method(ArduinoFake(), analogReference)).AlwaysReturn();                                           \
                When(Method(ArduinoFake(), delay)).AlwaysReturn();                                                     \
                When(Method(ArduinoFake(), millis)).AlwaysReturn(0);                                                   \
                When(Method(ArduinoFake(), micros)).AlwaysReturn(0);                                                   \
                When(Method(ArduinoFake(), cli)).AlwaysReturn();                                                       \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char))).AlwaysReturn();                       \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char*))).AlwaysReturn();                \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned char, int))).AlwaysReturn();         \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(int, int))).AlwaysReturn();                   \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long, int))).AlwaysReturn();                  \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(double, int))).AlwaysReturn();                \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned int, int))).AlwaysReturn();          \
                When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int))).AlwaysReturn();         \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t())).AlwaysReturn();                         \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(char))).AlwaysReturn();                     \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char*))).AlwaysReturn();              \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned char, int))).AlwaysReturn();       \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(int, int))).AlwaysReturn();                 \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(long, int))).AlwaysReturn();                \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(double, int))).AlwaysReturn();              \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned int, int))).AlwaysReturn();        \
                When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long, int))).AlwaysReturn();       \
                When(Method(ArduinoFake(Serial), end)).AlwaysReturn();                                                 \
                When(Method(ArduinoFake(Serial), flush)).AlwaysReturn();                                               \
                When(Method(ArduinoFake(Serial), available)).Return(0, 1);                                             \
                When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).Return(1);                         \
                When(OverloadedMethod(ArduinoFake(Serial), begin, void(unsigned long))).AlwaysReturn();                \
            }
#    else
#        define ARDUINO_MOCK_ALL()                                                                                     \
            {}

#    endif

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

    void set_state_impl(LogicalState state) {
        if (state == LogicalState::ON) digitalWrite(_cfg.pin, !_cfg.active_on_low);
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
#    if defined(EMBEDDED)
        analogWriteResolution(_cfg.resolution);
#    endif
        set_value(0.0);
    }

    // value between 0 and 1 where 1 is the logical state ON
    void set_value(double value) {
        assert(value >= 0.0 && value <= 1.0);
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
        //        assert(callback_actual);

        auto mode_actual = trigger != TriggerType::UNDEFINED ? trigger : _mode;
        //        assert(mode_actual != Mode::NOP);

        switch (mode_actual) {
        case TriggerType::RISING_AND_FALLING_EDGE: mode_bits = 1; break;
        case TriggerType::FALLING_EDGE: mode_bits = 2; break;
        case TriggerType::RISING_EDGE: mode_bits = 3; break;
        default: mode_bits = 0; break;
        }

        _mode = mode_actual;
        _callback = callback_actual;

#    if defined(__AVR_ATmega8__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
        ::attachInterrupt(_cfg.pin, callback_actual, mode_bits);
#    elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#    elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega1284P__)                    \
        || defined(__AVR_ATmega1284__)
#    elif defined(ARDUINO_SAM_DUE)
#    else
        assert(digitalPinToAnalogInput(_cfg.pin) != NOT_AN_INTERRUPT);
        ::attachInterrupt(digitalPinToInterrupt(_cfg.pin), callback_actual, mode_bits);
#    endif
    }

    void detach_interrupt() { ::detachInterrupt(_cfg.pin); }

private:
    const Config _cfg;

    TriggerType _mode;
    void (*_callback)();
};

class ArduinoI2C : public I2C<ArduinoI2C> {
public:
    // https://docs.particle.io/reference/device-os/api/wire-i2c/pins-i2c/
    struct Config {};

    ArduinoI2C(Config&& cfg) : _cfg(std::move(cfg)) {}

    static void initialize() { Wire.begin(); }
    static size_t available() { return Wire.available(); };
    static size_t read(uint8_t* buffer, uint16_t length) { return Wire.readBytes(buffer, length); }
    static size_t read() { return Wire.read(); }
    static size_t write(uint8_t* buffer, size_t length, uint8_t address) {
        Wire.beginTransmission(address);
        const auto bytes_written = Wire.write(buffer, length);
        if (const auto transmission_status = Wire.endTransmission(); transmission_status == 0) {
            return bytes_written;
        } else {
            DBG("Wire transmission failed with status: %i", transmission_status);
            return 0;
        }
    }

private:
    const Config _cfg;
};

// the minds of the Arduino team for some obscure reason have left an incomplete 'HardwareSerial' stream
// that doesnt implement 'availableForWrite'. We are left to wonder why a UART/USART class would inherit from a Serial
// class and not the other way around (Interface>Implementation). Since SAM is the only platform from Arduino's own
// lineup that is supported, we make a special case for this board.
#    if defined(SAM)
#        define UART_T ::UARTClass
#    else
#        define UART_T ::HardwareSerial
#    endif

class ArduinoUART : public UART<ArduinoUART> {
public:
    struct Config {
        ::Stream* stream; // Arduino HardwareSerial stream, typically 'Serial'
        time_ms timeout = time_ms(1000); //
    };

    ArduinoUART(Config&& cfg) : _cfg(std::move(cfg)), _stream_ref(static_cast<UART_T*>(_cfg.stream)) {}

    void initialize() override { _stream_ref->setTimeout(_cfg.timeout.raw<unsigned long>()); }

    size_t available() const override {
        assert(_stream_ref);
        return static_cast<size_t>(_stream_ref->available());
    };
    size_t available_for_write() const override {
        assert(_stream_ref);
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
    using AnalogueOutput = ArduinoAnalogueOutput;
    using AnalogueInput = ArduinoAnalogueInput;

    using Interrupt = ArduinoInterrupt;
};

struct ArduinoConfig {
    uint32_t baudrate;
};

struct Arduino : public Platform<Arduino, ArduinoConfig, ArduinoGPIO, ArduinoI2C, ArduinoUART> {
public:
    // PRINTING
    static void initialize(Config&& cfg) {
#    if defined(NATIVE)
        ARDUINO_MOCK_ALL();
#    endif
        if (cfg.baudrate > 0) {
            Serial.begin(cfg.baudrate);
        }

        // todo: set this up separately?
        analogReadResolution(10); // 0-1023
        analogWriteResolution(8); // 0-255
    };

    static void halt(const char* msg = NULL) {
        LOG("Halting with reason: %s", msg ? msg : "[not specified]");

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

    // TIMING
    static time_ms millis() { return time_ms(::millis()); }
    static time_us micros() { return time_us(::micros()); }
    static void delay_us(time_us us) { ::delayMicroseconds(us.raw<uint32_t>()); }
    static void delay_ms(time_ms ms) { ::delay(ms.raw<uint32_t>()); }

    static unsigned long free_memory();
};

} // namespace spn::platform

#endif
