#pragma once

#include "spine/structure/units/si.hpp"

#include <array>
#include <cinttypes>
#include <optional>
#include <string>
#include <variant>

namespace spn::core::utils {

/*
 * =========================================================================================
 * ChatGPT Generated Code
 * =========================================================================================
 *
 * This code was generated by OpenAI's ChatGPT, an AI language model trained to assist with
 * coding, documentation, and various programming tasks.
 *
 * Version: ChatGPT v2 (As of July 2024)
 * Model: GPT-4-turbo
 *
 * Note: While this code is intended to be useful, it's important to review and test it
 * thoroughly before integrating it into your project. Always ensure that it meets your
 * specific needs and adheres to best practices.
 *
 * =========================================================================================
 */

/// Converts a time value to a string representation with the appropriate unit.
/// Handles microseconds (us), milliseconds (ms), seconds (s), minutes (m),
/// hours (h), and days (d). Uses 900 seconds as the cutoff for minutes.
template<typename TimeType>
inline std::string repr(const TimeType& t) {
    auto buffer = std::array<char, 64>();
    // std::string_view unit;

    auto raw = (k_time_us(t).raw() < 1000)   ? k_time_us(t).raw()
               : (k_time_ms(t).raw() < 1000) ? k_time_ms(t).raw()
               : (k_time_s(t).raw() < 900)   ? k_time_s(t).raw() // use second up to 900s before switching to minute
               : (k_time_m(t).raw() < 1440)  ? k_time_m(t).raw() // 1440 minutes = 24 hours
               : (k_time_h(t).raw() < 24)    ? k_time_h(t).raw()
               : (k_time_d(t).raw() > 0)     ? k_time_d(t).raw()
                                             : 0;

    auto unit = (k_time_us(t).raw() < 1000)   ? "us"
                : (k_time_ms(t).raw() < 1000) ? "ms"
                : (k_time_s(t).raw() < 900)   ? "s"
                : (k_time_m(t).raw() < 1440)  ? "m"
                : (k_time_h(t).raw() < 24)    ? "h"
                : (k_time_d(t).raw() > 0)     ? "d"
                                              : "unknown";

    /// todo: this is a workaround, see issue with charconv: https://github.com/s-t-a-n/Spine/issues/21
    auto size = std::snprintf(buffer.data(), buffer.size(), "%" PRId64 "%s", static_cast<int64_t>(raw), unit);
    if (size < 0 || static_cast<size_t>(size) >= buffer.size()) {
        return "Error";
    }
    return std::string(buffer.data(), size);

    // auto [ptr, _] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), raw);
    // std::string result(ptr - buffer.data() + unit.size(), '\0');
    // std::memcpy(result.data(), buffer.data(), ptr - buffer.data());
    // std::memcpy(result.data() + (ptr - buffer.data()), unit.data(), unit.size());
    // return result;
}

/// Function to parse a string like "10us", "2h", or "1d" to a Time object
std::optional<std::variant<k_time_us, k_time_ms, k_time_s, k_time_m, k_time_h, k_time_d>>
parse_time(const std::string_view input);

} // namespace spn::core::utils
