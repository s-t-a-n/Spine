#pragma once

#ifdef ZEPHYR

#    define SPN_LOG_OVERLOAD(level, msg) spn::platform::zephyr_log(level, msg);

#    include "spine/core/debugging.hpp"
#    include "spine/core/logging.hpp"
#    include "spine/io/stream/stream.hpp"
#    include "spine/platform/platform.hpp"

#    include <sys/time.h>
#    include <zephyr/device.h>
#    include <zephyr/kernel.h>
#    include <zephyr/logging/log.h>

#    ifdef CONFIG_LOG_RUNTIME_FILTERING
#        define SPN_LOG_SET_LEVEL(level) spn::platform::zephyr_set_log_level(level);
#    endif

namespace spn::platform {

void zephyr_log(const spn::logging::LogLevel level, const char* msg);
void zephyr_set_log_level(const spn::logging::LogLevel level);

struct ZephyrConfig {};

struct Zephyr : Platform<Zephyr, ZephyrConfig> {
    static void initialize(Config&& cfg){};

    static void halt(const char* msg = nullptr) {
        if (msg) {
            SPN_WARN("System halt: %s", msg);
        }
        k_panic(); // Halt the system in Zephyr
    }

    static k_time_ms millis() { return k_time_ms(k_uptime_get()); }
    static k_time_us micros() { return k_time_us(k_ticks_to_us_near32(k_uptime_ticks())); }
    static void delay_us(k_time_us us) {
        if (us.raw() >= 1000) {
            k_msleep(us.raw() / 1000);
        } else {
            k_busy_wait(us.raw()); // Busy wait for shorter delays
        }
    }
    static void delay_ms(k_time_ms ms) { k_msleep(ms.raw()); }
};

} // namespace spn::platform

#endif
