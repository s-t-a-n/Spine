#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagAbsorbedDose {};
} // namespace spn::structure::units

// Absorbed dose, with unit: gray
using gray = spnu::Unit<spnu::TagAbsorbedDose, spnu::Base, spnu::RealRawType>;
