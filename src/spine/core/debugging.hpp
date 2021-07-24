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

void print(const char* filename, int line_number, const char* function_name, const char* msg);
void enable_dbg_output(bool enabled = true);
bool is_dbg_output_enabled();

#if defined(DEBUG_SPINE)
#    define assert(__SPINEDEBUG_condition)                                                                             \
        {                                                                                                              \
            if (!(__SPINEDEBUG_condition)) {                                                                           \
                if (dbg::is_dbg_output_enabled()) {                                                                    \
                    snprintf(dbg::__SPINEDEBUG_raw_msg, sizeof(dbg::__SPINEDEBUG_raw_msg), "Assertion failed: '%s'",   \
                             #__SPINEDEBUG_condition);                                                                 \
                    dbg::print(__FILE__, __LINE__, __func__, dbg::__SPINEDEBUG_raw_msg);                               \
                }                                                                                                      \
                spn::throw_exception(::spn::assertion_exception(dbg::__SPINEDEBUG_raw_msg));                           \
            }                                                                                                          \
        }

#    define DBG(...)                                                                                                   \
        {                                                                                                              \
            if (dbg::is_dbg_output_enabled()) {                                                                        \
                snprintf(dbg::__SPINEDEBUG_raw_msg, sizeof(dbg::__SPINEDEBUG_raw_msg), __VA_ARGS__);                   \
                dbg::print(__FILE__, __LINE__, __func__, dbg::__SPINEDEBUG_raw_msg);                                   \
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

namespace logging {

void print(const char* filename, int line_number, const char* function_name, const char* msg);
void enable_log_output(bool enabled = true);
bool is_log_output_enabled();

#define LOG(...)                                                                                                       \
    {                                                                                                                  \
        if (logging::is_log_output_enabled()) {                                                                        \
            snprintf(dbg::__SPINEDEBUG_raw_msg, sizeof(dbg::__SPINEDEBUG_raw_msg), __VA_ARGS__);                       \
            logging::print(__FILE__, __LINE__, __func__, dbg::__SPINEDEBUG_raw_msg);                                   \
        }                                                                                                              \
    }

} // namespace logging