#pragma once

#if defined(ARDUINO)

#    include "spine/platform/implementations/arduino.hpp"
using HAL = spn::platform::Platform<spn::platform::Arduino, //
                                    spn::platform::ArduinoConfig, //
                                    spn::platform::ArduinoGPIO, //
                                    spn::platform::ArduinoAnalogue, spn::platform::ArduinoUART>;
using ArduinoConfig = spn::platform::ArduinoConfig;

#elif defined(NATIVE)
#    include "spine/platform/implementations/mock.hpp"
using HAL = spn::platform::Platform<spn::platform::Mock, //
                                    spn::platform::MockConfig, //
                                    spn::platform::MockGPIO, //
                                    spn::platform::MockAnalogue, //
                                    spn::platform::MockUART>;
using MockConfig = spn::platform::MockConfig;

#elif defined(ZEPHYR)
#    include "spine/platform/implementations/zephyr.hpp"
using HAL = spn::platform::Platform<spn::platform::Zephyr, //
                                    spn::platform::ZephyrConfig>;
using ZephyrConfig = spn::platform::ZephyrConfig;
#else

#    error "Platform not supported"

#endif

#if defined(SPINE_PLATFORM_CAP_GPIO)
using DigitalOutput = HAL::DigitalOutput;
using DigitalInput = HAL::DigitalInput;
#endif

#if defined(SPINE_PLATFORM_CAP_ADC)
using AnalogueInput = HAL::AnalogueInput;
#endif

#if defined(SPINE_PLATFORM_CAP_PWM)
using AnalogueOutput = HAL::AnalogueOutput;
#endif

#if defined(SPINE_PLATFORM_CAP_GPIO_INTERRUPTS)
using Interrupt = HAL::Interrupt;
#endif