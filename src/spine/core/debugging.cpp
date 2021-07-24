#include "spine/core/debugging.hpp"

#include "spine/platform/hal.hpp"

namespace dbg {
#if defined(DEBUG_SPINE)
namespace {
char formatted_msg[512];
bool dbg_output_enabled = true;
} // namespace

namespace detail {
void print(const char* filename, const int line_number, const char* function_name, const char* msg) {
    if (!is_dbg_output_enabled()) return;
    snprintf(formatted_msg, sizeof(formatted_msg), "!%45s @ line %4i in function %-15s %8ums: %s", filename,
             line_number, function_name, static_cast<uint32_t>(HAL::millis().raw()), msg);
    HAL::println(formatted_msg);
    HAL::printflush();
}
} // namespace detail

void enable_dbg_output(bool enabled) { dbg_output_enabled = enabled; }
bool is_dbg_output_enabled() { return dbg_output_enabled; }
#else
namespace detail {
void print(const char* filename, const int line_number, const char* function_name, const char* msg) {}
} // namespace detail

void enable_dbg_output(bool enabled) {}
bool is_dbg_output_enabled() { return false; }
#endif
} // namespace dbg