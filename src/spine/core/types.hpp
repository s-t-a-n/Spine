#pragma once

namespace spn::core {

enum LogicalState { OFF, ON, UNDEFINED };
enum class TriggerType { RISING_EDGE, FALLING_EDGE, RISING_AND_FALLING_EDGE, UNDEFINED };

} // namespace spn::core