#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagCapacitance {};
} // namespace spn::structure::units

// Capacitance, with unit: farad
using farad_p = spnu::Unit<spnu::TagCapacitance, spnu::Pico, spnu::RealRawType>;
using farad_n = spnu::Unit<spnu::TagCapacitance, spnu::Nano, spnu::RealRawType>;
using farad_u = spnu::Unit<spnu::TagCapacitance, spnu::Micro, spnu::RealRawType>;
using farad_m = spnu::Unit<spnu::TagCapacitance, spnu::Milli, spnu::RealRawType>;
using farad = spnu::Unit<spnu::TagCapacitance, spnu::Base, spnu::RealRawType>;
