#include "spine/platform/gpio.hpp"

#include "spine/core/debugging.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/units/si.hpp"

namespace spn::platform::detail {

void fade_to(void (*set_value)(void*, float), void* imp, float value, float setpoint, float increment,
             k_time_ms increment_interval) {
    spn_assert(setpoint >= 0.0f && setpoint <= 1.0f);

    const float decimals_of_diff = 1.0f / increment;
    float diff = std::round(std::fabs(value - setpoint) * decimals_of_diff) / decimals_of_diff;

    if (diff < increment) {
        set_value(imp, setpoint);
        return;
    }
    const auto invert = value > setpoint ? -1 : 1;
    while (diff > 0) {
        value = std::clamp(value + invert * increment, 0.0f, 1.0f);
        set_value(imp, value);
        diff -= increment;
        HAL::delay(increment_interval);
    }
}

} // namespace spn::platform::detail
