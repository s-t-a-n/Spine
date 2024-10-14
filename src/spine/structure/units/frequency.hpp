#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagFrequency {};
} // namespace spn::structure::units

// Frequency, with unit: hertz
using hertz = spnu::Unit<spnu::TagFrequency, spnu::Base, spnu::RealRawType>;
using hertz_k = spnu::Unit<spnu::TagFrequency, spnu::Kilo, spnu::RealRawType>;
using hertz_M = spnu::Unit<spnu::TagFrequency, spnu::Mega, spnu::RealRawType>;
using hertz_G = spnu::Unit<spnu::TagFrequency, spnu::Giga, spnu::RealRawType>;
