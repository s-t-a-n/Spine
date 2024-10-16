#pragma once

#include "spine/structure/units/primitives/unit.hpp"

#include <cmath>
#include <cstdint>
#include <ctime>

namespace spn::structure::units {
struct TagTemperatureKelvin {};
struct TagTemperatureCelsius {};
} // namespace spn::structure::units

// Temperature, with unit: kelvin
using kelvin = spnu::Unit<spnu::TagTemperatureKelvin, spnu::Base, spnu::RealRawType>;

// Temperature, with unit: Celsius
using celsius = spnu::Unit<spnu::TagTemperatureCelsius, spnu::Base, spnu::RealRawType>;

/// Convert a spnu::kelvin to spnu::celsius
inline celsius convert_to_celsius(kelvin t) { return celsius(t.raw() - 273.15); }
inline celsius convert_to_celsius(celsius t) { return t; }

/// Convert a spnu::celsius to spnu::kelvin
inline kelvin convert_to_kelvin(celsius t) { return kelvin(t.raw() + 273.15); }
inline kelvin convert_to_kelvin(kelvin t) { return t; }