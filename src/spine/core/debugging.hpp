#pragma once

#include "spine/core/exception.hpp"

#include <cstdio>

#if defined(DEBUG_SPINE)

#    include "spine/core/logging.hpp"

#endif

#ifdef assert
#    undef assert
#endif

#ifdef expect
#    undef expect
#endif

#ifdef DBG
#    error "DBG is already defined"
#endif

#if defined(DEBUG_SPINE)

#    include "spine/core/logging.hpp"

/// Macro to run assertions which, if failing, will log the failed assertion and throw a spn::exception
#    define assert(__SPINEDEBUG_condition)                                                                             \
        {                                                                                                              \
            if (!(__SPINEDEBUG_condition)) {                                                                           \
                ERR("Assertion failed: '%s'", #__SPINEDEBUG_condition);                                                \
                spn::throw_exception(::spn::assertion_exception(#__SPINEDEBUG_condition));                             \
            }                                                                                                          \
        }

/// Macro to run expectations which, if failing, will log the failed expectation
#    define expect(__SPINEDEBUG_condition)                                                                             \
        {                                                                                                              \
            if (!(__SPINEDEBUG_condition)) {                                                                           \
                ERR("Expectation failed: '%s'", #__SPINEDEBUG_condition);                                              \
            }                                                                                                          \
        }

/// Macro to print debugging statements, output is prefixed with '!'
#    define DBG(...) LOG_IMPL(spn::logging::LogLevel::DEBUG, __VA_ARGS__)
#else
#    define assert(condition)                                                                                          \
        do {                                                                                                           \
        } while (0);
#    define expect(condition)                                                                                          \
        do {                                                                                                           \
        } while (0);
#    define DBG(...)                                                                                                   \
        do {                                                                                                           \
        } while (0);
#endif
