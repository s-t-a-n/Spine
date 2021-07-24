#include <spine/core/debugging.hpp>

int main() {
    using namespace spn::core;

    DBG("This is a debug messagee. Used to signal internal behaviour to a developer. It only prints when the "
        "preprocessor `SPINE_DEBUG` is set during build");

    assert(true); // if not  true, an exception will be raised. See exception.cpp for further details on that.

    return 0;
}