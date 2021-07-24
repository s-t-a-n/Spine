#include "spine/platform/implementations/arduino.hpp"

#if defined(ARDUINO)

namespace spn::platform {

// ripped from: https://github.com/mpflaga/Arduino-MemoryFree/tree/master
#    ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#    else // __ARM__
extern char* __brkval;
#    endif // __arm__

unsigned long Arduino::free_memory() {
    char top;
#    ifdef __arm__
    return &top - reinterpret_cast<char*>(sbrk(0));
#    elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#    elif not defined(NATIVE) // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#    else
    return 0;
#    endif
}

} // namespace spn::platform

#endif