#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagRadioactivity {};
} // namespace spn::structure::units

// Radioactivity, with unit: becquerel
using becquerel = spnu::Unit<spnu::TagRadioactivity, spnu::Base, spnu::RealRawType>;
