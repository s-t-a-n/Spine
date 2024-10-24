#include "spine/core/utils/time_repr.hpp"

// #include <charconv>
#include <iterator>
#include <numeric>
#include <optional>
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

// Function to parse a string like "10us", "2h", or "1d" to a Time object
std::optional<std::variant<k_time_us, k_time_ms, k_time_s, k_time_m, k_time_h, k_time_d>>
parse_time(const std::string_view input) {
    if (input.empty()) return std::nullopt;

    // Find the position of the first non-digit character
    std::size_t pos = input.find_first_not_of("0123456789");
    if (pos == std::string_view::npos) return std::nullopt; // No unit found

    // Extract numeric value and unit
    int value = std::stoi(std::string(input.substr(0, pos)));
    std::string_view unit = input.substr(pos);

    if (unit == "us") return k_time_us(value);
    if (unit == "ms") return k_time_ms(value);
    if (unit == "s") return k_time_s(value);
    if (unit == "m") return k_time_m(value);
    if (unit == "h") return k_time_h(value);
    if (unit == "d") return k_time_d(value);

    return std::nullopt; // Unknown or unsupported unit
}

} // namespace spn::core::utils
