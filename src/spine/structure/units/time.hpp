#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

namespace spn::structure::units {
struct TagTime {};
} // namespace spn::structure::units

// Time, with unit: second
using time_us = spnu::Unit<spnu::TagTime, spnu::Micro, spnu::TimeRawType>;
using time_ms = spnu::Unit<spnu::TagTime, spnu::Milli, spnu::TimeRawType>;
using time_s = spnu::Unit<spnu::TagTime, spnu::Base, spnu::TimeRawType>;
using time_m = spnu::Unit<spnu::TagTime, spnu::Minute, spnu::TimeRawType>;
using time_h = spnu::Unit<spnu::TagTime, spnu::Hour, spnu::TimeRawType>;
using time_d = spnu::Unit<spnu::TagTime, spnu::Day, spnu::TimeRawType>;
