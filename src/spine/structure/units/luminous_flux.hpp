#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagLuminousFlux {};
} // namespace spn::structure::units

// Luminous flux, with unit: lumen
using lumen = spnu::Unit<spnu::TagLuminousFlux, spnu::Base, spnu::RealRawType>;
