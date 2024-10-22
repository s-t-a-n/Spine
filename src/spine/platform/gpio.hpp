#pragma once

#include "spine/core/types.hpp"
#include "spine/structure/units/si.hpp"

namespace spn::platform {

template<typename GPIOImp>
class DigitalOutput {
public:
    using LogicalState = core::LogicalState;

    void initialize(bool active = false) {
        static_cast<GPIOImp*>(this)->initialize_impl();
        set_state(active);
    }
    void set_state(core::LogicalState state) {
        static_cast<GPIOImp*>(this)->set_state_impl(state);
        _active = state;
    }
    void set_state(bool active) { set_state(static_cast<core::LogicalState>(active)); }
    void flip() { set_state(!state()); }
    LogicalState state() const { return _active; }

private:
    LogicalState _active = LogicalState::OFF;
};

template<typename GPIOImp>
struct DigitalInput {
    using LogicalState = core::LogicalState;

    void initialize() { static_cast<GPIOImp>(this)->initialize(); }
    LogicalState state() const { return static_cast<GPIOImp>(this)->state(); }
};

namespace detail {
void fade_to(void (*set_value)(void*, float), void* imp, float value, float setpoint, float increment,
             k_time_ms increment_interval);
}

template<typename GPIOImp>
class AnalogueOutput {
public:
    /// Initialize a port/pin
    void initialize() { static_cast<GPIOImp>(this)->initialize(); }

    /// Set the output to the provided `value`
    void set_value(float value) {
        _value = value;
        ((GPIOImp*)(this))->set_value(value);
    }

    /// Returns the currently active value
    float value() const { return _value; }

    /// Set a `value` in a (short) amount of time to reduce sudden rushes of current
    void fade_to(float setpoint, float increment = 0.1, k_time_ms increment_interval = k_time_ms(250)) {
        const auto set_v = [](void* imp, float value) -> void { ((AnalogueOutput*)imp)->set_value(value); };
        spn::platform::detail::fade_to(set_v, (void*)this, _value, setpoint, increment, increment_interval);
        _value = setpoint;
    }

private:
    float _value = 0;
};

template<typename GPIOImp>
struct AnalogueInput {
    /// Initialize an analogue input port/pin
    void initialize() { static_cast<GPIOImp>(this)->initialize(); }

    /// Reads the Analogue input port/pin
    float read() { return static_cast<GPIOImp>(this)->read(); }
};

// todo: Add an ADC/DAC configuration blocks to set up stuff like read/write resolution, ref voltage and sampling
// frequency

template<typename GPIOImp>
struct Interrupt {
    using TriggerType = core::TriggerType;

    /// Intializes the interrupt (doesn't attach it)
    void initialize() { static_cast<GPIOImp>(this)->initialize(); }

    /// Attaches the interrupt to the callback.
    void attach_interrupt(void (*callback)() = nullptr, TriggerType trigger = TriggerType::UNDEFINED) {
        static_cast<GPIOImp>(this)->attach_interrupt(callback, trigger);
    }

    /// Detaches the interrupt from the callback
    void detach_interrupt() { static_cast<GPIOImp>(this)->detach_interrupt(); }
};

} // namespace spn::platform
