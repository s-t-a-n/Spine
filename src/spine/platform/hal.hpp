#pragma once

#include "spine/platform/platform.hpp"

#if defined(ARDUINO)
#    include "spine/platform/implementations/arduino.hpp"
using HAL = spn::platform::Platform<spn::platform::Arduino, //
                                    spn::platform::ArduinoConfig, //
                                    spn::platform::ArduinoGPIO, //
                                    spn::platform::ArduinoI2C, //
                                    spn::platform::ArduinoUART>;
using ArduinoConfig = spn::platform::ArduinoConfig;

#elif defined(NATIVE)
#    include "spine/platform/implementations/mock.hpp"
using HAL = spn::platform::Platform<spn::platform::Mock, //
                                    spn::platform::MockConfig, //
                                    spn::platform::MockGPIO, //
                                    spn::platform::MockI2C, //
                                    spn::platform::MockUART>;
using MockConfig = spn::platform::MockConfig;

#endif

using DigitalOutput = HAL::DigitalOutput;
using DigitalInput = HAL::DigitalInput;
using AnalogueOutput = HAL::AnalogueOutput;
using AnalogueInput = HAL::AnalogueInput;

using Interrupt = HAL::Interrupt;