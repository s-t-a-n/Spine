#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

#include <cmath>
#include <cstdint>
#include <ctime>

namespace spn::structure::units {
struct TagCatalyticActivity {};
} // namespace spn::structure::units

// Catalytic activity, with unit: katal
using katal = spnu::Unit<spnu::TagCatalyticActivity, spnu::Base, spnu::RealRawType>;
