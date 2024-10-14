#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagVolume {};
} // namespace spn::structure::units

// Volume, with unit: litre
using litre_ml = spnu::Unit<spnu::TagVolume, spnu::Milli, spnu::RealRawType>;
using litre = spnu::Unit<spnu::TagVolume, spnu::Base, spnu::RealRawType>;
