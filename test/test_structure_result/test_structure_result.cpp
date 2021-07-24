#include "spine/structure/result.hpp"

#include <unity.h>

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

using namespace spn::structure;

namespace {

using namespace spn::core;

void ut_result_basics() {
    using ParserResult = Result<double>;
    const auto stage_entry = [](double input) -> ParserResult { return ParserResult(input); };
    const auto stage_count = [](ParserResult&& p) -> ParserResult {
        if (p.is_failed()) return p;
        return ParserResult{p.value() + 1};
    };
    const auto stage_fail = [](ParserResult&& p) -> ParserResult { return ParserResult::failed({}); };

    auto simple_result = stage_count(stage_entry(1));
    TEST_ASSERT_EQUAL(true, bool(simple_result));
    TEST_ASSERT_EQUAL(2.0, *simple_result);

    auto failed_result = stage_count(stage_fail(stage_entry(1)));
    TEST_ASSERT_EQUAL(false, bool(failed_result));
    TEST_ASSERT_EQUAL(true, failed_result.is_failed());
}

void ut_result_parser() {
    class Parser {
    public:
        using ParserResult = Result<double, std::string>;

        Parser() {}

        ParserResult parse(double input) {
            _result = stage_entry(input);
            _result = stage_double();
            return _result;
        }
        ParserResult parse_with_error(double input) {
            _result = stage_entry(input);
            _result = stage_error();
            _result = stage_double();
            return _result;
        }

    protected:
        ParserResult stage_entry(double input) { return {input}; }
        ParserResult stage_double() {
            if (_result.is_failed()) return _result;

            if (_result.value() == 0) return ParserResult::failed("stage_double: value was 0");
            return {_result.value() * 2};
        }
        ParserResult stage_error() {
            if (_result.is_failed()) return _result;
            return ParserResult::failed("stage_error");
        }

    private:
        ParserResult _result;
    };

    Parser p;
    const auto result = p.parse(5);
    TEST_ASSERT_EQUAL(true, result.is_success());
    TEST_ASSERT_EQUAL(false, result.is_failed());
    TEST_ASSERT_EQUAL(10, result.value());

    const auto result_error = p.parse_with_error(5);
    TEST_ASSERT_EQUAL(false, result_error.is_success());
    TEST_ASSERT_EQUAL(true, result_error.is_failed());
    TEST_ASSERT_EQUAL_STRING("stage_error", result_error.error_value().c_str());

    const auto result_conditional_error = p.parse(0);
    TEST_ASSERT_EQUAL(false, result_conditional_error.is_success());
    TEST_ASSERT_EQUAL(true, result_conditional_error.is_failed());
    TEST_ASSERT_EQUAL_STRING("stage_double: value was 0", result_conditional_error.error_value().c_str());
}

void ut_result_parser_chain() {
    class Parser {
    public:
        using ParseResult = spn::structure::Result<double, std::string, int>;

        Parser() {}

        ParseResult parse(double input) {
            return stage_entry(input).chain([this](double value) { return stage_double(value); });
        }

        ParseResult parse_with_error(double input) {
            return stage_entry(input)
                .chain([this](double value) { return stage_error(value); })
                .chain([this](double value) { return stage_double(value); });
        }

        ParseResult parse_with_early_success(double input) {
            return stage_entry(input)
                .chain([this](double value) { return stage_early_success(value); })
                .chain([this](double value) { return stage_error(value); })
                .chain([this](double value) { return stage_double(value); });
        }

    protected:
        ParseResult stage_entry(double input) { return ParseResult::intermediary(input); }

        ParseResult stage_double(double value) {
            if (value == 0) {
                return ParseResult::failed("stage_double: value was 0");
            }
            return ParseResult(value * 2);
        }

        ParseResult stage_error(double value) { return ParseResult::failed("stage_error"); }
        ParseResult stage_early_success(double value) { return ParseResult(42); }
    };

    Parser p;
    const auto result = p.parse(5);
    TEST_ASSERT_EQUAL(true, result.is_success());
    TEST_ASSERT_EQUAL(false, result.is_intermediary());
    TEST_ASSERT_EQUAL(false, result.is_failed());
    TEST_ASSERT_EQUAL(10, result.value());

    const auto result_error = p.parse_with_error(5);
    TEST_ASSERT_EQUAL(false, result_error.is_success());
    TEST_ASSERT_EQUAL(false, result_error.is_intermediary());
    TEST_ASSERT_EQUAL(true, result_error.is_failed());
    TEST_ASSERT_EQUAL_STRING("stage_error", result_error.error_value().c_str());

    const auto result_conditional_error = p.parse(0);
    TEST_ASSERT_EQUAL(false, result_conditional_error.is_success());
    TEST_ASSERT_EQUAL(false, result_conditional_error.is_intermediary());
    TEST_ASSERT_EQUAL(true, result_conditional_error.is_failed());
    TEST_ASSERT_EQUAL_STRING("stage_double: value was 0", result_conditional_error.error_value().c_str());

    const auto result_early_success = p.parse_with_early_success(5);
    TEST_ASSERT_EQUAL(true, result_early_success.is_success());
    TEST_ASSERT_EQUAL(false, result_early_success.is_intermediary());
    TEST_ASSERT_EQUAL(false, result_early_success.is_failed());
    TEST_ASSERT_EQUAL(42, result_early_success.value());
}

void ut_result_parser_chain_intermediary() {
    class Parser {
    public:
        struct ParserHandle {
            explicit ParserHandle(double v) : value(v) {}
            double value;
        };
        using ParseResult = spn::structure::Result<double, std::string, ParserHandle>;

        Parser() {}

        ParseResult parse(double input) {
            return stage_entry(input)
                .chain([this](ParserHandle handle) { return stage_n(handle); })
                .chain([this](ParserHandle handle) { return stage_n(handle); })
                .chain([this](ParserHandle handle) { return stage_final(handle); });
        }

    protected:
        ParseResult stage_entry(double input) { return ParseResult::intermediary(ParserHandle(input)); }

        ParseResult stage_n(ParserHandle handle) {
            if (handle.value == 0) {
                return ParseResult::failed("stage_double: value was 0");
            }
            return ParseResult::intermediary(ParserHandle{handle.value + 1});
        }

        ParseResult stage_final(ParserHandle handle) { return ParseResult(handle.value); }
    };
    Parser p;
    const auto result = p.parse(5);

    TEST_ASSERT_EQUAL(7, result.value());
}

void ut_result_parser_chain_single_type() {
    class Parser {
    public:
        struct Message {
            Message(const std::string& v) : value(v) {}
            Message(const std::string&& v) : value(v) {}
            std::string value;
        };
        using ParseResult = Result<Message>;

        Parser() {}

        ParseResult parse(const std::string& input) {
            return stage_entry(input)
                .chain([this](Message handle) { return stage_n(handle); })
                .chain([this](Message handle) { return stage_n(handle); })
                .chain([this](Message handle) { return stage_final(handle); });
        }

    protected:
        ParseResult stage_entry(const std::string& input) { return ParseResult::intermediary(Message(input)); }

        ParseResult stage_n(Message handle) {
            if (handle.value == "world") {
                return ParseResult::failed(Message("stage_n: value was 'world'"));
            }
            return ParseResult::intermediary(Message{handle.value + " world"});
        }

        ParseResult stage_final(Message handle) { return ParseResult(handle.value); }
    };
    Parser p;
    const auto result = p.parse("hello");

    TEST_ASSERT_EQUAL_STRING("hello world world", result.value().value.c_str());
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_result_basics);
    RUN_TEST(ut_result_parser);
    RUN_TEST(ut_result_parser_chain);
    RUN_TEST(ut_result_parser_chain_intermediary);
    RUN_TEST(ut_result_parser_chain_single_type);
    return UNITY_END();
}

#if defined(ARDUINO) && defined(EMBEDDED)
#    include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    run_all_tests();
}

void loop() {}
#elif defined(ARDUINO)
#    include <ArduinoFake.h>
#endif

int main(int argc, char** argv) {
#if defined(ARDUINO)
    ARDUINO_MOCK_ALL();
#endif
    run_all_tests();
    return 0;
}
