#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagConductance {};
} // namespace spn::structure::units

// Conductance, with unit: siemens
using siemens_u = spnu::Unit<spnu::TagConductance, spnu::Micro, spnu::RealRawType>;
using siemens_m = spnu::Unit<spnu::TagConductance, spnu::Milli, spnu::RealRawType>;
using siemens = spnu::Unit<spnu::TagConductance, spnu::Base, spnu::RealRawType>;
