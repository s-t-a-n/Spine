#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagPower {};
} // namespace spn::structure::units

// Power, with unit: watt
using watt_m = spnu::Unit<spnu::TagPower, spnu::Milli, spnu::RealRawType>;
using watt = spnu::Unit<spnu::TagPower, spnu::Base, spnu::RealRawType>;
using watt_k = spnu::Unit<spnu::TagPower, spnu::Kilo, spnu::RealRawType>;
using watt_M = spnu::Unit<spnu::TagPower, spnu::Mega, spnu::RealRawType>;
using watt_G = spnu::Unit<spnu::TagPower, spnu::Giga, spnu::RealRawType>;
