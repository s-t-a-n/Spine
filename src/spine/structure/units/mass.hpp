#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagMass {};
} // namespace spn::structure::units

// Mass, with unit: gram
using gram_u = spnu::Unit<spnu::TagMass, spnu::Micro, spnu::RealRawType>;
using gram_m = spnu::Unit<spnu::TagMass, spnu::Milli, spnu::RealRawType>;
using gram = spnu::Unit<spnu::TagMass, spnu::Base, spnu::RealRawType>;
using gram_k = spnu::Unit<spnu::TagMass, spnu::Kilo, spnu::RealRawType>;
