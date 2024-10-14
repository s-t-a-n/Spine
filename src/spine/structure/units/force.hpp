#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagForce {};
} // namespace spn::structure::units

// Force, with unit: newton
using newton = spnu::Unit<spnu::TagForce, spnu::Base, spnu::RealRawType>;
using newton_k = spnu::Unit<spnu::TagForce, spnu::Kilo, spnu::RealRawType>;
