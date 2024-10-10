#include "spine/core/logging.hpp"

#include "spine/platform/hal.hpp"

namespace spn::logging {

namespace {
constexpr const char* to_string(LogLevel level) {
    switch (level) {
    case LogLevel::OFF: return "OFF";
    case LogLevel::ERR: return "ERR";
    case LogLevel::WARN: return "WARN";
    case LogLevel::INFO: return "INFO";
    case LogLevel::DEBUG: return "DBG";
    default: return "UNKNOWN";
    }
}

volatile LogLevel g_log_level =
#ifdef DEBUG_SPINE
    LogLevel::DEBUG;
#else
    LogLevel::INFO;
#endif

} // namespace

namespace detail {

const char* get_current_time() {
    static char time_buffer[32];
    snprintf(time_buffer, sizeof(time_buffer), "%08u", static_cast<uint32_t>(HAL::millis().raw()));
    return time_buffer;
}

void print(const LogLevel level, const char* filename, const int line_number, const char* function_name,
           const char* fmt, ...) {
    char msg_buffer[SPINE_DEBUG_BUFFER_SIZE];
    size_t space_used = 0;

#ifndef SPN_LOG_OVERLOAD
    space_used += snprintf(msg_buffer, sizeof(msg_buffer), "[%s] [%s] ", get_current_time(), to_string(level));
#endif

    va_list args;
    va_start(args, fmt);
    space_used += vsnprintf(msg_buffer + space_used, sizeof(msg_buffer) - space_used, fmt, args);
    va_end(args);

    space_used += snprintf(msg_buffer + space_used, sizeof(msg_buffer) - space_used, " (%s:%d %s)", filename,
                           line_number, function_name);

#if defined(SPN_LOG_OVERLOAD)
    SPN_LOG_OVERLOAD(level, msg_buffer);
#elif defined(SPINE_PLATFORM_CAP_PRINT) && defined(SPINE_PLATFORM_CAP_PRINTF)
    HAL::println(msg_buffer);
    HAL::printflush();
#else
#    error "Platform doesnt provide print capacity, nor was an overload of SPN_LOG_OVERLOAD provided."
#endif
}
} // namespace detail

void set_log_level(const LogLevel level) {
    g_log_level = level;
#ifdef SPN_LOG_SET_LEVEL
    SPN_LOG_SET_LEVEL(level)
#endif
}
LogLevel log_level() { return g_log_level; }
bool is_loggable(const LogLevel level) { return level <= g_log_level; }
} // namespace spn::logging