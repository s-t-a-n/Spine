#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagInductance {};
} // namespace spn::structure::units

// Inductance, with unit: henry
using henry = spnu::Unit<spnu::TagInductance, spnu::Base, spnu::RealRawType>;
