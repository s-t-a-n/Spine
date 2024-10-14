#include <spine/core/logging.hpp>
#include <spine/structure/units/energy.hpp>
#include <spine/structure/units/time.hpp>

// see unittests for more examples

int main() {
    auto my_energy = joule(150);
    SPN_LOG("my_energy by default: %.2f", my_energy.raw())

    my_energy += joule_m(100);
    SPN_LOG("my_energy with 0.1J more: %.2f", my_energy.raw())

    my_energy -= joule_G(1e-9);
    SPN_LOG("my_energy with 1J less: %.2f", my_energy.raw())

    auto my_energy_multiplied = my_energy * 3;
    SPN_LOG("my_energy_multiplied by 3: %.2f", my_energy_multiplied.raw())

    auto my_time = time_ms(100);
    SPN_LOG("my_time: %.2f ms", my_time.raw())

    auto my_kernel_time = k_time_ms(100);
    SPN_LOG("my_kernel_time: %li ms", my_kernel_time.raw())

    return 0;
}