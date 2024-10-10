#include <spine/core/debugging.hpp>

int main() {
    using namespace spn::core;

    SPN_DBG("This is a debug messagee. Used to signal internal behaviour to a developer. It only prints when the "
            "preprocessor `SPINE_DEBUG` is set during build");

    spn_assert(true); // if condition is not met, an exception will be raised. See exception.cpp for further details.
    spn_expect(true); // if condition is not met, an error will be logged.
    return 0;
}
