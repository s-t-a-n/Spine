#pragma once

#include "spine/core/exception.hpp"

#include <cstdio>

namespace dbg {
inline static char __SPINEDEBUG_raw_msg[256];

#ifdef assert
#    undef assert
#endif

#ifdef DBG
#    error "DBG is already defined"
#endif

namespace detail {
void print(const char* filename, int line_number, const char* function_name, const char* msg);
}

/// Enable or disable debug output globally
void enable_dbg_output(bool enabled = true);

/// Returns true if debug output is enabled globally (default: true)
bool is_dbg_output_enabled();

#if defined(DEBUG_SPINE)

/// Macro to do assertions which if failing will output the failed assertion and throw a spn::exception
#    define assert(__SPINEDEBUG_condition)                                                                             \
        {                                                                                                              \
            if (!(__SPINEDEBUG_condition)) {                                                                           \
                if (dbg::is_dbg_output_enabled()) {                                                                    \
                    snprintf(dbg::__SPINEDEBUG_raw_msg, sizeof(dbg::__SPINEDEBUG_raw_msg), "Assertion failed: '%s'",   \
                             #__SPINEDEBUG_condition);                                                                 \
                    dbg::detail::print(__FILE__, __LINE__, __func__, dbg::__SPINEDEBUG_raw_msg);                       \
                }                                                                                                      \
                spn::throw_exception(::spn::assertion_exception(dbg::__SPINEDEBUG_raw_msg));                           \
            }                                                                                                          \
        }

/// Macro to print debugging statements, output is prefixed with '!'
#    define DBG(...)                                                                                                   \
        {                                                                                                              \
            if (dbg::is_dbg_output_enabled()) {                                                                        \
                snprintf(dbg::__SPINEDEBUG_raw_msg, sizeof(dbg::__SPINEDEBUG_raw_msg), __VA_ARGS__);                   \
                dbg::detail::print(__FILE__, __LINE__, __func__, dbg::__SPINEDEBUG_raw_msg);                           \
            }                                                                                                          \
        }
#else
#    define assert(condition)                                                                                          \
        do {                                                                                                           \
        } while (0);
#    define DBG(...)                                                                                                   \
        do {                                                                                                           \
        } while (0);
#endif

} // namespace dbg