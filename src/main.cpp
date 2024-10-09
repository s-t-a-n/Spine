#include "spine/controller/pid.hpp"
#include "spine/controller/sr_latch.hpp"
#include "spine/core/debugging.hpp"
#include "spine/core/exception.hpp"
#include "spine/core/meta/enum.hpp"
#include "spine/core/meta/unique_type_variant.hpp"
#include "spine/core/utils/concatenate.hpp"
#include "spine/core/utils/string.hpp"
#include "spine/core/utils/time_repr.hpp"
#include "spine/eventsystem/eventsystem.hpp"
#include "spine/filter/filter.hpp"
#include "spine/filter/filterstack.hpp"
#include "spine/filter/implementations/bandpass.hpp"
#include "spine/filter/implementations/ewma.hpp"
#include "spine/filter/implementations/invert.hpp"
#include "spine/filter/implementations/mapped_range.hpp"
#include "spine/filter/implementations/passthrough.hpp"
#include "spine/io/sensor.hpp"
#include "spine/io/stream/buffered_stream.hpp"
#include "spine/io/stream/implementations/mock.hpp"
#include "spine/io/stream/stream.hpp"
#include "spine/io/stream/transaction.hpp"
#include "spine/platform/gpio.hpp"
#include "spine/platform/hal.hpp"
#include "spine/platform/protocols/uart.hpp"
#include "spine/structure/array.hpp"
#include "spine/structure/linebuffer.hpp"
#include "spine/structure/point.hpp"
#include "spine/structure/pool.hpp"
#include "spine/structure/result.hpp"
#include "spine/structure/ringbuffer.hpp"
#include "spine/structure/stack.hpp"
#include "spine/structure/static_string.hpp"
#include "spine/structure/time/datetime.hpp"
#include "spine/structure/time/schedule.hpp"
#include "spine/structure/time/timers.hpp"
#include "spine/structure/tuple.hpp"
#include "spine/structure/units/si.hpp"
#include "spine/structure/vector.hpp"

void setup() {
    HAL::initialize(HAL::Config{}); // defaults to a baudrate of 115200 if HAL provides UART
    HAL::delay_ms(500); // give time for monitor to connect after flashing
    SPN_LOG("Wake up");

    SPN_LOG("End of setup. Entering main program.");
}

void loop() {
    SPN_DBG("loop");
    HAL::delay_ms(1000);
}

#if defined(NATIVE) and !defined(UNITTEST)
int main(int argc, char** argv) {
    setup();
    while (true) {
        loop();
    }
    return 0;
}
#endif
