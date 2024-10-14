#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagCurrent {};
} // namespace spn::structure::units

// Current, with unit: ampere
using amp_u = spnu::Unit<spnu::TagCurrent, spnu::Micro, spnu::RealRawType>;
using amp_m = spnu::Unit<spnu::TagCurrent, spnu::Milli, spnu::RealRawType>;
using amp = spnu::Unit<spnu::TagCurrent, spnu::Base, spnu::RealRawType>;
