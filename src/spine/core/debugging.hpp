#pragma once

#include "spine/core/exception.hpp"

#ifdef spn_assert
#    error "spn_assert is already defined"
#endif

#ifdef spn_expect
#    error "spn_expect is already defined"
#endif

#ifdef SPN_DBG
#    error "SPN_DBG is already defined"
#endif

#if defined(DEBUG_SPINE)

#    include "spine/core/logging.hpp"

/// Macro to run assertions which, if failing, will log the failed assertion and throw a spn::exception
#    define spn_assert(__SPINEDEBUG_condition)                                                                         \
        {                                                                                                              \
            if (!(__SPINEDEBUG_condition)) {                                                                           \
                SPN_ERR("Assertion failed: '%s'", #__SPINEDEBUG_condition);                                            \
                spn::throw_exception(::spn::assertion_exception(#__SPINEDEBUG_condition));                             \
            }                                                                                                          \
        }

/// Macro to run expectations which, if failing, will log the failed expectation
#    define spn_expect(__SPINEDEBUG_condition)                                                                         \
        {                                                                                                              \
            if (!(__SPINEDEBUG_condition)) {                                                                           \
                SPN_ERR("Expectation failed: '%s'", #__SPINEDEBUG_condition);                                          \
            }                                                                                                          \
        }

/// Macro to print debugging statements. Only is compiled in when SPINE_DEBUG is defined.
#    define SPN_DBG(...) SPN_LOG_IMPL(spn::logging::LogLevel::DEBUG, __VA_ARGS__)
#else
#    define spn_assert(condition)                                                                                      \
        do {                                                                                                           \
        } while (0);
#    define spn_expect(condition)                                                                                      \
        do {                                                                                                           \
        } while (0);
#    define SPN_DBG(...)                                                                                               \
        do {                                                                                                           \
        } while (0);
#endif

// Alias for SPN_DBG if it was not previously defined.
#ifndef DBG
#    define DBG(...) SPN_DBG(__VA_ARGS__)
#endif
