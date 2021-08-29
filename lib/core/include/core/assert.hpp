#pragma once

#include <core/standard.hpp>
#include <core/system.hpp>

#ifdef assert
#    undef assert
#endif

#ifdef DEBUG
#    define assert(condition)                                                                                          \
        if (!condition) {                                                                                              \
            noInterrupts();                                                                                            \
            spn::core::debug_print(__FILE__, __LINE__, __func__, "assertion failed!");                                 \
            spn::core::terminate();                                                                                    \
        }
#else
#    define assert(condition)                                                                                          \
        {                                                                                                              \
            do {                                                                                                       \
            } while (0);                                                                                               \
        }
#endif
