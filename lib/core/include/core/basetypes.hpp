#pragma once

#include <stdint.h>

namespace Spine::Actuators {

/// type of integer that can address all pins;
// clang-format off
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega
    using pin_t = uint8_t;
    using size_t = uint8_t;
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__ || __AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__) // uno / leonardo
    using pin_t = uint8_t;
    using size_t = uint8_t;
#endif
// clang-format on

} // namespace Spine::Actuators