#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagEnergy {};
} // namespace spn::structure::units

// Energy, with unit: joule
using joule_p = spnu::Unit<spnu::TagEnergy, spnu::Pico, spnu::RealRawType>;
using joule_n = spnu::Unit<spnu::TagEnergy, spnu::Nano, spnu::RealRawType>;
using joule_u = spnu::Unit<spnu::TagEnergy, spnu::Micro, spnu::RealRawType>;
using joule_m = spnu::Unit<spnu::TagEnergy, spnu::Milli, spnu::RealRawType>;
using joule = spnu::Unit<spnu::TagEnergy, spnu::Base, spnu::RealRawType>;
using joule_k = spnu::Unit<spnu::TagEnergy, spnu::Kilo, spnu::RealRawType>;
using joule_M = spnu::Unit<spnu::TagEnergy, spnu::Mega, spnu::RealRawType>;
using joule_G = spnu::Unit<spnu::TagEnergy, spnu::Giga, spnu::RealRawType>;
using joule_T = spnu::Unit<spnu::TagEnergy, spnu::Tera, spnu::RealRawType>;
