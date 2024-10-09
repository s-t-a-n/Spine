#pragma once

#ifdef ZEPHYR

#    include "spine/core/debugging.hpp"
#    include "spine/io/stream/stream.hpp"
#    include "spine/platform/platform.hpp"

#    include <logging/log.h>
#    include <sys/time.h>
#    include <zephyr.h>

namespace spn::platform {

struct ZephyrConfig {};

struct Zephyr : Platform<Zephyr, ZephyrConfig> {
    static void initialize(Config&& cfg){};

    static void halt(const char* msg = nullptr) {
        if (msg) {
            SPN_LOG_ERR("System halt: %s", msg);
        }
        k_panic(); // Halt the system in Zephyr
    }

    static time_ms millis() { return time_ms(k_uptime_get()); }
    static time_us micros() { return time_us(k_ticks_to_us_near32(k_uptime_ticks())); }
    static void delay_us(time_us us) {
        if (us.raw() >= 1000) {
            k_msleep(us / 1000);
        } else {
            k_busy_wait(us.raw()); // Busy wait for shorter delays
        }
    }
    static void delay_ms(time_ms ms) { k_msleep(ms.raw()); }
};

} // namespace spn::platform

#endif
