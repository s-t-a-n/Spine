#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagAmountOfSubstance {};
} // namespace spn::structure::units

// Amount of substance, with unit: mole
using mole = spnu::Unit<spnu::TagAmountOfSubstance, spnu::Base, spnu::RealRawType>;
