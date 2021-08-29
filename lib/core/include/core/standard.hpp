#pragma once

/// standard system header includes
#include <stdint.h>

#ifdef ARDUINO
#    include <Arduino.h>
#endif

/// standard typedefs with global scope

/// standard typedefs with spn::core scope
namespace spn::core {

}

/// type of integer that can address all pins;
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP8266) // 8-bit
#elif defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) // 32-bit
#endif
