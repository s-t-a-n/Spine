#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagMagneticFlux {};
} // namespace spn::structure::units

// Magnetic flux, with unit: weber
using weber = spnu::Unit<spnu::TagMagneticFlux, spnu::Base, spnu::RealRawType>;
