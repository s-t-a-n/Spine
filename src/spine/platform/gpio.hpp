#pragma once

#include "spine/core/si_units.hpp"

enum LogicalState { OFF, ON, UNDEFINED };
enum class TriggerType { RISING_EDGE, FALLING_EDGE, RISING_AND_FALLING_EDGE, UNDEFINED };

namespace spn::platform {

template<typename GPIOImp>
class DigitalOutput {
public:
    void initialize(bool active = false) {
        static_cast<GPIOImp*>(this)->initialize_impl();
        set_state(active);
    }
    void set_state(LogicalState state) {
        static_cast<GPIOImp*>(this)->set_state_impl(state);
        _active = state;
    }
    void set_state(bool active) { set_state(static_cast<LogicalState>(active)); }
    void flip() { set_state(!state()); }
    [[nodiscard]] bool state() const { return _active; }

private:
    bool _active = false;
};

template<typename GPIOImp>
struct DigitalInput {
    void initialize() { static_cast<GPIOImp>(this)->initialize(); }
    [[nodiscard]] bool state() const { return static_cast<GPIOImp>(this)->state(); }
};

namespace detail {
void fade_to(void (*set_value)(void*, double), void* imp, double value, double setpoint, double increment,
             time_ms increment_interval);
}
template<typename GPIOImp>
class AnalogueOutput {
public:
    void initialize() { static_cast<GPIOImp>(this)->initialize(); }
    void set_value(double value) {
        _value = value;
        ((GPIOImp*)(this))->set_value(value);
    }

    double value() const { return _value; }

    void fade_to(double setpoint, double increment = 0.1, time_ms increment_interval = time_ms(250)) {
        const auto set_v = [](void* imp, double value) -> void { ((AnalogueOutput*)imp)->set_value(value); };
        spn::platform::detail::fade_to(set_v, (void*)this, _value, setpoint, increment, increment_interval);
        _value = setpoint;
    }

private:
    double _value = 0;
};

template<typename GPIOImp>
struct AnalogueInput {
    void initialize() { static_cast<GPIOImp>(this)->initialize(); }
    // todo: find a way to set stuff like read/write resolution and frequency for analog
    // void set_reference_voltage(double v, bool external = false) {
    // static_cast<GPIOImp>(this)->set_reference_voltage(v);
    // }
    double read() { return static_cast<GPIOImp>(this)->read(); }
};

template<typename GPIOImp>
struct Interrupt {
    using TriggerType = ::TriggerType;

    void initialize() { static_cast<GPIOImp>(this)->initialize(); }
    void attach_interrupt(void (*callback)() = nullptr, TriggerType trigger = TriggerType::UNDEFINED) {
        static_cast<GPIOImp>(this)->attach_interrupt(callback, trigger);
    }
    void detach_interrupt() { static_cast<GPIOImp>(this)->detach_interrupt(); }
};

} // namespace spn::platform
