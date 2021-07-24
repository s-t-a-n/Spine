#include "spine/core/logging.hpp"

#include "spine/platform/hal.hpp"

namespace logging {

namespace {
char formatted_msg[512];
bool log_output_enabled = true;
} // namespace

namespace detail {
void print(const char* filename, const int line_number, const char* function_name, const char* msg) {
    if (!is_log_output_enabled()) return;
    snprintf(formatted_msg, sizeof(formatted_msg), "#%45s @ line %4i in function %-15s %8ums: %s", filename,
             line_number, function_name, static_cast<uint32_t>(HAL::millis().raw()), msg);
    HAL::println(formatted_msg);
    HAL::printflush();
}
} // namespace detail

void enable_log_output(bool enabled) { log_output_enabled = enabled; }
bool is_log_output_enabled() { return log_output_enabled; }

} // namespace logging