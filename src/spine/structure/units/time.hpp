#pragma once

#include "spine/structure/units/primitives/magnitudes.hpp"
#include "spine/structure/units/primitives/raw_types.hpp"
#include "spine/structure/units/primitives/unit.hpp"

#include <cmath>

namespace spn::structure::units {
struct TagTime {};
struct TagKernelTime {};
} // namespace spn::structure::units

// Real time, with unit: second
using time_us = spnu::Unit<spnu::TagTime, spnu::Micro, spnu::TimeRawType>;
using time_ms = spnu::Unit<spnu::TagTime, spnu::Milli, spnu::TimeRawType>;
using time_s = spnu::Unit<spnu::TagTime, spnu::Base, spnu::TimeRawType>;
using time_m = spnu::Unit<spnu::TagTime, spnu::Minute, spnu::TimeRawType>;
using time_h = spnu::Unit<spnu::TagTime, spnu::Hour, spnu::TimeRawType>;
using time_d = spnu::Unit<spnu::TagTime, spnu::Day, spnu::TimeRawType>;
using time_W = spnu::Unit<spnu::TagTime, spnu::Week, spnu::TimeRawType>;
using time_M = spnu::Unit<spnu::TagTime, spnu::Month, spnu::TimeRawType>;
using time_Y = spnu::Unit<spnu::TagTime, spnu::Year, spnu::TimeRawType>;

// Kernel time, with unit: second
// Pay attention: `k_time_s(k_time_ms(500) == 0` holds true, since kernel time works on an int64
using k_time_us = spnu::Unit<spnu::TagKernelTime, spnu::Micro, spnu::KernelTimeRawType>;
using k_time_ms = spnu::Unit<spnu::TagKernelTime, spnu::Milli, spnu::KernelTimeRawType>;
using k_time_s = spnu::Unit<spnu::TagKernelTime, spnu::Base, spnu::KernelTimeRawType>;
using k_time_m = spnu::Unit<spnu::TagKernelTime, spnu::Minute, spnu::KernelTimeRawType>;
using k_time_h = spnu::Unit<spnu::TagKernelTime, spnu::Hour, spnu::KernelTimeRawType>;
using k_time_d = spnu::Unit<spnu::TagKernelTime, spnu::Day, spnu::KernelTimeRawType>;

template<typename M>
/// Returns a kernel_time for a provided time. Second argument is a rounding function such as std::floor, std::round or
/// std::ceil. Defaults to std::ceil such that a time_ms(1.5) returns k_time_ms(2) as that is considered safest.
inline spnu::Unit<spnu::TagKernelTime, M, spnu::KernelTimeRawType>
to_kernel_time(const spnu::Unit<spnu::TagTime, M, spnu::TimeRawType> other,
               spnu::TimeRawType (*rounding_func)(spnu::TimeRawType) = std::ceil) {
    return spnu::Unit<spnu::TagKernelTime, M, spnu::KernelTimeRawType>(rounding_func(other.raw())); // bankers' rounding
}

template<typename M>
/// Returns a real `time` for a kernel time `k_time`.
inline spnu::Unit<spnu::TagTime, M, spnu::TimeRawType>
to_real_time(const spnu::Unit<spnu::TagKernelTime, M, spnu::KernelTimeRawType> other) {
    return spnu::Unit<spnu::TagTime, M, spnu::TimeRawType>(other.raw());
}