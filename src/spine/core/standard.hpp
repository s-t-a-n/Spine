#pragma once

/// standard system header includes
// #include <cstdint>

#include "spine/platform/hal.hpp"

#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

// undo some of Arduino's annoying syntax sugar
#if defined(abs)
#    undef abs
#endif
#if defined(LOW)
#    undef LOW
#endif
#if defined(HIGH)
#    undef HIGH
#endif
#if defined(RISING)
#    undef RISING
#endif
#if defined(FALLING)
#    undef FALLING
#endif
#if defined(CHANGE)
#    undef CHANGE
#endif
#if defined(DEFAULT)
#    undef DEFAULT
#endif
#if defined(EXTERNAL)
#    undef EXTERNAL
#endif

/// global macros
#define ENUM_IDX(__ENUM_IDX) static_cast<int>(__ENUM_IDX)

/// standard typedefs with global scope

/// standard typedefs with spn::core scope
namespace spn::core {

} // namespace spn::core
