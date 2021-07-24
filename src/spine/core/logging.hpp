#pragma once

#include "spine/core/exception.hpp"

#include <cstdio>
namespace logging {

namespace detail {
void print(const char* filename, int line_number, const char* function_name, const char* msg);
} // namespace detail

/// Enable or disable logging output globally
void enable_log_output(bool enabled = true);

/// Returns true if logging output is enabled globally (default: true)
bool is_log_output_enabled();

#ifdef LOG
#    error "LOG is already defined"
#endif

/// Macro to print logging statements, output is prefixed with '#'
#define LOG(...)                                                                                                       \
    {                                                                                                                  \
        if (logging::is_log_output_enabled()) {                                                                        \
            snprintf(dbg::__SPINEDEBUG_raw_msg, sizeof(dbg::__SPINEDEBUG_raw_msg), __VA_ARGS__);                       \
            logging::detail::print(__FILE__, __LINE__, __func__, dbg::__SPINEDEBUG_raw_msg);                           \
        }                                                                                                              \
    }

} // namespace logging