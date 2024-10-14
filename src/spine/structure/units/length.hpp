#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagLength {};
} // namespace spn::structure::units

// Length, with unit: meter
using meter_n = spnu::Unit<spnu::TagLength, spnu::Nano, spnu::RealRawType>;
using meter_u = spnu::Unit<spnu::TagLength, spnu::Micro, spnu::RealRawType>;
using meter_m = spnu::Unit<spnu::TagLength, spnu::Milli, spnu::RealRawType>;
using meter_c = spnu::Unit<spnu::TagLength, spnu::Centi, spnu::RealRawType>;
using meter_d = spnu::Unit<spnu::TagLength, spnu::Deci, spnu::RealRawType>;
using meter = spnu::Unit<spnu::TagLength, spnu::Base, spnu::RealRawType>;
using meter_k = spnu::Unit<spnu::TagLength, spnu::Kilo, spnu::RealRawType>;
