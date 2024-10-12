#ifdef NATIVE

#    include "spine/platform/implementations/mock.hpp"

namespace spn::platform {

MockState& MockStateInstance() {
    static MockState ms{};
    return ms;
}

} // namespace spn::platform

#endif