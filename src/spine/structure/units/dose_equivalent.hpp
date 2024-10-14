#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagDoseEquivalent {};
} // namespace spn::structure::units

// Dose equivalent, with unit: sievert
using sievert = spnu::Unit<spnu::TagDoseEquivalent, spnu::Base, spnu::RealRawType>;
