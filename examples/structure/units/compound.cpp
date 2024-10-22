#include <spine/core/logging.hpp>
#include <spine/structure/units/primitives/compound_unit.hpp>
#include <spine/structure/units/time.hpp>
#include <spine/structure/units/volume.hpp>

int main() {
    using namespace spnu;
    using mls = CompoundUnit<litre_ml, time_s>;
    using lpm = CompoundUnit<litre, time_m>;

    auto flowrate = mls(litre_ml(100), time_s(20));
    SPN_LOG("flow rate: %.2f mL/s", flowrate.raw());
    flowrate += lpm(litre_ml(100), time_s(20));
    SPN_LOG("flow rate: %.2f L/m", lpm(flowrate).raw());

    return 0;
}
