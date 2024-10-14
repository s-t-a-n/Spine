#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagCharge {};
} // namespace spn::structure::units

// Electric charge, with unit: coulomb
using coulomb_n = spnu::Unit<spnu::TagCharge, spnu::Nano, spnu::RealRawType>;
using coulomb_u = spnu::Unit<spnu::TagCharge, spnu::Micro, spnu::RealRawType>;
using coulomb_m = spnu::Unit<spnu::TagCharge, spnu::Milli, spnu::RealRawType>;
using coulomb = spnu::Unit<spnu::TagCharge, spnu::Base, spnu::RealRawType>;
