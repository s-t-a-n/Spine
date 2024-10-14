#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagPotential {};
} // namespace spn::structure::units

// Potential, with unit: volt
using volt_u = spnu::Unit<spnu::TagPotential, spnu::Micro, spnu::RealRawType>;
using volt_m = spnu::Unit<spnu::TagPotential, spnu::Milli, spnu::RealRawType>;
using volt = spnu::Unit<spnu::TagPotential, spnu::Base, spnu::RealRawType>;
