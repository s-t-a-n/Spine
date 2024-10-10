#ifdef ZEPHYR

#    include "spine/platform/implementations/zephyr.hpp"

#    include <zephyr/logging/log.h>

#    ifdef DEBUG_SPINE
LOG_MODULE_REGISTER(spine, LOG_LEVEL_DBG);
#    else
LOG_MODULE_REGISTER(spine, LOG_LEVEL_INF);
#    endif

namespace spn::platform {

void zephyr_log(const spn::logging::LogLevel level, const char* msg) {
    switch (level) {
    case spn::logging::LogLevel::ERR: LOG_ERR("%s", msg); break;
    case spn::logging::LogLevel::WARN: LOG_WRN("%s", msg); break;
    case spn::logging::LogLevel::INFO: LOG_INF("%s", msg); break;
    case spn::logging::LogLevel::DEBUG: LOG_DBG("%s", msg); break;
    default: break;
    }
}

#    ifdef CONFIG_LOG_RUNTIME_FILTERING
#        include <zephyr/logging/log_ctrl.h>

void zephyr_set_log_level(spn::logging::LogLevel level) {
    LOG_MODULE_DECLARE(spine, LOG_LEVEL_INF);

    switch (level) {
    case spn::logging::LogLevel::OFF: log_filter_set(log_backend_get_by_name("spine"), 0, 0, LOG_LEVEL_NONE);
    case spn::logging::LogLevel::ERR: log_filter_set(log_backend_get_by_name("spine"), 0, 0, LOG_LEVEL_ERR); break;
    case spn::logging::LogLevel::WARN: log_filter_set(log_backend_get_by_name("spine"), 0, 0, LOG_LEVEL_WRN); break;
    case spn::logging::LogLevel::INFO: log_filter_set(log_backend_get_by_name("spine"), 0, 0, LOG_LEVEL_INF); break;
    case spn::logging::LogLevel::DEBUG: log_filter_set(log_backend_get_by_name("spine"), 0, 0, LOG_LEVEL_DBG); break;
    default: break;
    }
}
#    endif
} // namespace spn::platform

#endif