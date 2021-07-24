#pragma once

#include <Arduino.h>

// Undefine Arduino-specific macros if they are defined
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

// Undefine standard library macros if they are defined
#if defined(abs)
#    undef abs
#endif

#if defined(min)
#    undef min
#endif

#if defined(max)
#    undef max
#endif

#if defined(constrain)
#    undef constrain
#endif

#if defined(round)
#    undef round
#endif

#if defined(UART)
#    undef UART
#endif

#if defined(SERIAL)
#    undef SERIAL
#endif
