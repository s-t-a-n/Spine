#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagPressure {};
} // namespace spn::structure::units

// Pressure, with unit: pascal
using pascal = spnu::Unit<spnu::TagPressure, spnu::Base, spnu::RealRawType>;
using pascal_k = spnu::Unit<spnu::TagPressure, spnu::Kilo, spnu::RealRawType>;
using pascal_M = spnu::Unit<spnu::TagPressure, spnu::Mega, spnu::RealRawType>;
