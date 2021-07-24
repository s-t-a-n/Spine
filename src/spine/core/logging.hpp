#pragma once

#include "spine/core/exception.hpp"

#ifndef SPINE_DEBUG_BUFFER_SIZE
#    define SPINE_DEBUG_BUFFER_SIZE 512
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

#ifdef LOG_IMPL
#    error "LOG_IMPL is already defined"
#endif

#define LOG_IMPL(level, ...)                                                                                           \
    {                                                                                                                  \
        if (spn::logging::is_loggable(level)) {                                                                        \
            spn::logging::detail::print(level, __FILE__, __LINE__, __func__, __VA_ARGS__);                             \
        }                                                                                                              \
    }
} // namespace detail

} // namespace spn::logging

#ifdef LOG
#    error "LOG is already defined"
#endif

/// Logging macro for informational messages
#define LOG(...) LOG_IMPL(spn::logging::LogLevel::INFO, __VA_ARGS__)

#ifdef WARN
#    error "WARN is already defined"
#endif

/// Logging macro for warning messages
#define WARN(...) LOG_IMPL(spn::logging::LogLevel::WARN, __VA_ARGS__)

#ifdef ERR
#    error "ERR is already defined"
#endif

/// Logging macro for error messages
#define ERR(...) LOG_IMPL(spn::logging::LogLevel::ERR, __VA_ARGS__)