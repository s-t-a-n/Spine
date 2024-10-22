#pragma once

#include <ctime>

namespace spn::structure::units {

using IntegerRawType = long long;
using KernelTimeRawType = time_t;

#ifdef SPN_PLATFORM_CAP_DOUBLE
using TimeRawType = double;
using RealRawType = double;
#else
using TimeRawType = float;
using RealRawType = float;
#endif

} // namespace spn::structure::units
