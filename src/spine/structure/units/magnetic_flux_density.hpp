#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagMagneticFluxDensity {};
} // namespace spn::structure::units

// Magnetic flux density, with unit: tesla
using tesla = spnu::Unit<spnu::TagMagneticFluxDensity, spnu::Base, spnu::RealRawType>;
