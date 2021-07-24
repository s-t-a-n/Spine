#include "spine/platform/gpio.hpp"

#include "spine/core/debugging.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/units/si.hpp"

namespace spn::platform::detail {

void fade_to(void (*set_value)(void*, double), void* imp, double value, double setpoint, double increment,
             time_ms increment_interval) {
    assert(setpoint >= 0.0 && setpoint <= 1.0);

    const double decimals_of_diff = 1.0 / increment;
    double diff = std::round(std::fabs(value - setpoint) * decimals_of_diff) / decimals_of_diff;

    if (diff < increment) {
        set_value(imp, setpoint);
        return;
    }
    const auto invert = value > setpoint ? -1 : 1;
    while (diff > 0) {
        value = std::clamp(value + invert * increment, 0.0, 1.0);
        set_value(imp, value);
        diff -= increment;
        HAL::delay(increment_interval);
    }
}

} // namespace spn::platform::detail