#include <core/assert.hpp>
#include <core/exception.hpp>
#include <core/standard.hpp>

void setup() {
    // write your initialization code here
}

void loop() {
    assert(false);

    spn_throw(spn::core::runtime_error("hello"));
    // write your code here
}