#pragma once

#include <list>

namespace spn::structure {

// couldn't be bothered; it is assumed that such lists exist for more developed RISC-like platforms

template<typename T>
using List = std::list<T>;

} // namespace spn::structure