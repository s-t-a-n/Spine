#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagIlluminance {};
} // namespace spn::structure::units

// Illuminance, with unit: lux
using lux = spnu::Unit<spnu::TagIlluminance, spnu::TagIlluminance, spnu::RealRawType>;
