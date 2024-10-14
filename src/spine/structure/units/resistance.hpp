#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagResistance {};
} // namespace spn::structure::units

// Resistance, with unit: ohm
using ohm_m = spnu::Unit<spnu::TagResistance, spnu::Milli, spnu::RealRawType>;
using ohm = spnu::Unit<spnu::TagResistance, spnu::Base, spnu::RealRawType>;
using ohm_k = spnu::Unit<spnu::TagResistance, spnu::Kilo, spnu::RealRawType>;
using ohm_M = spnu::Unit<spnu::TagResistance, spnu::Mega, spnu::RealRawType>;
