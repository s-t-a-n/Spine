#pragma once

namespace spn::core {

#ifdef ARDUINO
/// print to serial output on arduino
inline void debug_print(const char* filename, const int line_number, const char* function_name, const char* message) {
    if (!Serial) {
        return;
    }
    char str[256];

    snprintf(str, sizeof(str), "%s @ line %i in function %s: %s", filename, line_number, function_name, message);
    Serial.println(str);
}

#else
inline void debug_print(const char*, const int, const char*, const char*) {}
#endif

/// completely stop execution
inline void terminate() {
    noInterrupts();
    while (true) {
    };
}

} // namespace spn::core