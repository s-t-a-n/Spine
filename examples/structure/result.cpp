#include <spine/core/debugging.hpp>
#include <spine/structure/result.hpp>

// errorcode that will be used as an example below
enum class ErrorCode : uint8_t { None = 0, SomethingWentWrong, ValueTooSmall, InvalidIntermediary };

const char* error_code_to_string(ErrorCode code) {
    switch (code) {
    case ErrorCode::None: return "None";
    case ErrorCode::SomethingWentWrong: return "Something went wrong";
    case ErrorCode::ValueTooSmall: return "Value too small";
    case ErrorCode::InvalidIntermediary: return "Invalid intermediary value";
    default: return "Unknown error";
    }
}

int main() {
    using namespace spn::structure;

    // Example: Basic Success and Error Handling using ErrorCode
    Result<int, ErrorCode> success_result(10);
    Result<int, ErrorCode> error_result = Result<int, ErrorCode>::failed(ErrorCode::SomethingWentWrong);

    if (success_result.is_success()) SPN_DBG("Success result value: %d\n", success_result.value());
    if (error_result.is_failed()) SPN_DBG("Error result: %s\n", error_code_to_string(error_result.error_value()));

    // Example: Intermediary state handling using ErrorCode
    Result<int, ErrorCode, int> intermediary_result = Result<int, ErrorCode, int>::intermediary(5);

    if (intermediary_result.is_intermediary())
        SPN_DBG("Intermediary result value: %d\n", intermediary_result.intermediary_value());

    // Example: Using map to transform success value
    auto mapped_result = success_result.map([](const int& value) {
        return value * 2; // Doubles the success value
    });
    if (mapped_result.is_success()) SPN_DBG("Mapped success value: %d\n", mapped_result.value());

    // Example: Using map_error to transform error value (modify error codes if needed)
    auto mapped_error_result = error_result.map_error([](ErrorCode err) {
        // You could change error codes here if needed
        return ErrorCode::SomethingWentWrong;
    });
    if (mapped_error_result.is_failed())
        SPN_DBG("Mapped error result: %s\n", error_code_to_string(mapped_error_result.error_value()));

    // Example: Chaining operations with and_then
    auto chained_result = success_result.and_then([](const int& value) -> Result<int, ErrorCode> {
        if (value > 5) return Result<int, ErrorCode>(value * 3); // Transform if success
        return Result<int, ErrorCode>::failed(ErrorCode::ValueTooSmall);
    });
    if (chained_result.is_success()) SPN_DBG("Chained result success: %d\n", chained_result.value());

    // Example: Using or_else to handle error and provide a fallback
    auto recovered_result = error_result.or_else([](ErrorCode err) -> Result<int, ErrorCode> {
        SPN_DBG("Recovering from error: %s\n", error_code_to_string(err));
        return Result<int, ErrorCode>(42); // Provide a fallback success value
    });
    if (recovered_result.is_success()) SPN_DBG("Recovered result success: %d\n", recovered_result.value());

    // Example: Using chain to process intermediary state
    auto final_result = intermediary_result
                            .chain([](int intermediary_value) {
                                if (intermediary_value > 0)
                                    return Result<int, ErrorCode, int>(intermediary_value + 10); // Promote to success
                                return Result<int, ErrorCode, int>::failed(ErrorCode::InvalidIntermediary);
                            })
                            .chain([](int intermediary_value) {
                                if (intermediary_value > 0)
                                    return Result<int, ErrorCode, int>(intermediary_value + 10); // Promote to success
                                return Result<int, ErrorCode, int>::failed(ErrorCode::InvalidIntermediary);
                            });
    if (final_result.is_success()) SPN_DBG("Final result success: %d\n", final_result.value());

    return 0;
}
