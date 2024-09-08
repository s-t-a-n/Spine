#include "spine/controller/pid.hpp"
#include "spine/controller/sr_latch.hpp"
#include "spine/core/datetime.hpp"
#include "spine/core/debugging.hpp"
#include "spine/core/exception.hpp"
#include "spine/core/schedule.hpp"
#include "spine/core/si_units.hpp"
#include "spine/core/timers.hpp"
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
#include "spine/io/stream/implementations/uart.hpp"
#include "spine/io/stream/stream.hpp"
#include "spine/io/stream/transaction.hpp"
#include "spine/platform/gpio.hpp"
#include "spine/platform/hal.hpp"
#include "spine/platform/implementations/arduino.hpp"
#include "spine/platform/platform.hpp"
#include "spine/platform/protocols/i2c.hpp"
#include "spine/platform/protocols/uart.hpp"
#include "spine/structure/array.hpp"
#include "spine/structure/blockbuffer.hpp"
#include "spine/structure/linebuffer.hpp"
#include "spine/structure/list.hpp"
#include "spine/structure/point.hpp"
#include "spine/structure/pool.hpp"
#include "spine/structure/result.hpp"
#include "spine/structure/ringbuffer.hpp"
#include "spine/structure/rwbuffer.hpp"
#include "spine/structure/stack.hpp"
#include "spine/structure/static_string.hpp"
#include "spine/structure/tuple.hpp"
#include "spine/structure/vector.hpp"
#include "spine/tracker/average_change.hpp"
#include "spine/tracker/run_away.hpp"

void setup() {
    HAL::initialize(HAL::Config{.baudrate = 115200});
    HAL::delay_ms(100);
    HAL::println("wake up");

    DBG("End of setup");
}

void loop() {
    DBG("loop");
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
