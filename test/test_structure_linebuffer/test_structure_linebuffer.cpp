#include <limits.h>
#include <spine/structure/linebuffer.hpp>
#include <unity.h>

#include <cstdint>
#include <cstdlib>
#include <vector>

using namespace spn::structure;

namespace {

void ut_linebuffer_basics() {
    constexpr auto test_size = 100;
    LineBuffer line_buffer(test_size);

    // sanity checks
    TEST_ASSERT_EQUAL(0, line_buffer.used_space());
    TEST_ASSERT_EQUAL(test_size, line_buffer.free_space());
    TEST_ASSERT_EQUAL(0, line_buffer.length_of_next_line());

    line_buffer.push("abc\ndef\nghi\n");

    auto buffer = std::vector<char>(test_size);
    size_t chars_read;

    // test next line view
    auto sv = line_buffer.get_next_line_view();
    TEST_ASSERT_EQUAL(true, bool(sv));
    TEST_ASSERT_EQUAL(0, sv->compare("abc"));
    TEST_ASSERT_EQUAL(3, sv->size());

    // test basic splitting
    TEST_ASSERT_EQUAL(true, line_buffer.has_line());
    chars_read = line_buffer.get_next_line(buffer.data(), buffer.size());
    TEST_ASSERT_EQUAL(3, chars_read);
    TEST_ASSERT_EQUAL_STRING("abc", buffer.data());

    TEST_ASSERT_EQUAL(true, line_buffer.has_line());
    chars_read = line_buffer.get_next_line(buffer.data(), buffer.size());
    TEST_ASSERT_EQUAL(3, chars_read);
    TEST_ASSERT_EQUAL_STRING("def", buffer.data());

    TEST_ASSERT_EQUAL(true, line_buffer.has_line());
    chars_read = line_buffer.get_next_line(buffer.data(), buffer.size());
    TEST_ASSERT_EQUAL(3, chars_read);
    TEST_ASSERT_EQUAL_STRING("ghi", buffer.data());

    TEST_ASSERT_EQUAL(false, line_buffer.has_line());
    TEST_ASSERT_EQUAL(true, line_buffer.empty());
}

void ut_linebuffer_various_strings() {
    //
    const auto test = [](std::string_view input, std::vector<std::string> expected) {
        LineBuffer line_buffer(input.size());
        auto read_buffer = std::vector<char>(input.size());
        size_t chars_read;

        line_buffer.push(input);

        for (size_t i = 0; i < expected.size(); ++i) {
            chars_read = line_buffer.get_next_line(read_buffer.data(), read_buffer.size());
            TEST_ASSERT_EQUAL(expected[i].size(), chars_read);
            TEST_ASSERT_EQUAL_STRING(expected[i].c_str(), std::string(read_buffer.data(), chars_read).c_str());
        }
    };

    test("abc\ndef\nghi\n", {"abc", "def", "ghi"});
    test("\n\n\n", {"", "", ""});
    test("ab\n\n\n", {"ab", "", ""});
    test("\nab\n\n", {"", "ab", ""});
    test("\n\nab\n", {"", "", "ab"});
    test("\n\n\nab", {"", "", ""});
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_linebuffer_basics);
    RUN_TEST(ut_linebuffer_various_strings);
    return UNITY_END();
}

#if defined(ARDUINO)
#    include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    run_all_tests();
}

void loop() {}
#else
int main(int argc, char** argv) {
    run_all_tests();
    return 0;
}
#endif
