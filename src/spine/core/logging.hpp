#pragma once

#include "spine/core/exception.hpp"

#ifndef SPINE_DEBUG_BUFFER_SIZE
#    define SPINE_DEBUG_BUFFER_SIZE 256
#endif

namespace spn::logging {

enum class LogLevel { OFF, ERR, WARN, INFO, DEBUG };

/// Set the log level to any `LogLevel`
void set_log_level(LogLevel level);

/// Returns current `LogLevel`
LogLevel log_level();

/// Returns true if `level` is enabled
bool is_loggable(LogLevel level);

namespace detail {
void print(LogLevel level, const char* filename, int line_number, const char* function_name, const char* fmt, ...);
} // namespace detail

/// A user overloadable logging macro that takes a log level (see above) and a printf-style statement
#define SPN_LOG_IMPL(level, ...)                                                                                       \
    {                                                                                                                  \
        _Pragma("GCC diagnostic push");                                                                                \
        _Pragma("GCC diagnostic ignored \"-Wdouble-promotion\"");                                                      \
        if (spn::logging::is_loggable(level)) {                                                                        \
            spn::logging::detail::print(level, __FILE__, __LINE__, __func__, __VA_ARGS__);                             \
        }                                                                                                              \
        _Pragma("GCC diagnostic pop");                                                                                 \
    }

} // namespace spn::logging

/*
 * LOG: an event that is logged to keep track of the machines normal operation.
 */

#ifdef SPN_LOG
#    error "SPN_LOG is already defined"
#endif

/// Logging macro for informational messages
#define SPN_LOG(...) SPN_LOG_IMPL(spn::logging::LogLevel::INFO, __VA_ARGS__)

// Alias for SPN_LOG if it was not previously defined.
#ifndef LOG
#    define LOG(...) SPN_LOG(__VA_ARGS__)
#endif

/*
 * WARN: a recoverable problem that can occur under normal operation but is logged to keep track of.
 */

#ifdef SPN_WARN
#    error "SPN_WARN is already defined"
#endif

/// Logging macro for warning messages
#define SPN_WARN(...) SPN_LOG_IMPL(spn::logging::LogLevel::WARN, __VA_ARGS__)

// Alias for SPN_WARN if it was not previously defined.
#ifndef WARN
#    define WARN(...) SPN_WARN(__VA_ARGS__)
#endif

/*
 * ERROR: a recoverable or unrecoverable problem that should not occur under normal operation.
 */

#ifdef SPN_ERR
#    error "SPN_ERR is already defined"
#endif

/// Logging macro for error messages
#define SPN_ERR(...) SPN_LOG_IMPL(spn::logging::LogLevel::ERR, __VA_ARGS__)

// Alias for SPN_ERR if it was not previously defined.
#ifndef ERR
#    define ERR(...) SPN_ERR(__VA_ARGS__)
#endif
