#include <limits.h>
#include <spine/structure/ringbuffer.hpp>
#include <unity.h>

#include <cstdint>
#include <cstdlib>
#include <vector>

using namespace spn::structure;

namespace {

void ut_ringbuffer_basics() {
    constexpr auto test_size = 10;
    RingBuffer<int> buffer(test_size);

    TEST_ASSERT_EQUAL(test_size, buffer.capacity());

    TEST_ASSERT_EQUAL(false, buffer.full());
    TEST_ASSERT_EQUAL(0, buffer.used_space());
    TEST_ASSERT_EQUAL(test_size, buffer.free_space());
    TEST_ASSERT_EQUAL(true, buffer.empty());

    // fill up buffer
    for (size_t i = 0; i < test_size; ++i) {
        buffer.push(i);
    }

    TEST_ASSERT_EQUAL(true, buffer.full());
    TEST_ASSERT_EQUAL(test_size, buffer.used_space());
    TEST_ASSERT_EQUAL(0, buffer.free_space());
    TEST_ASSERT_EQUAL(false, buffer.empty());

    // verify peek
    for (size_t i = 0; i < test_size; ++i) {
        int v;
        TEST_ASSERT_EQUAL(true, buffer.peek_at(v, i));
        TEST_ASSERT_EQUAL(i, v);
    }

    // verify peek range
    int* ptr;
    TEST_ASSERT_EQUAL(true, buffer.peek_at(&ptr, 0));
    for (size_t i = 0; i < test_size; ++i) {
        TEST_ASSERT_EQUAL(i, ptr[i]);
    }

    { // test out of range peek
        int v;
        TEST_ASSERT_EQUAL(false, buffer.peek_at(v, test_size));
    }

    // empty out buffer
    for (size_t i = 0; i < test_size; ++i) {
        int v;
        TEST_ASSERT_EQUAL(true, buffer.pop(v));
        TEST_ASSERT_EQUAL(i, v);
    }

    TEST_ASSERT_EQUAL(false, buffer.full());
    TEST_ASSERT_EQUAL(0, buffer.used_space());
    TEST_ASSERT_EQUAL(test_size, buffer.free_space());
    TEST_ASSERT_EQUAL(true, buffer.empty());

    // fill without rollover
    constexpr auto extended_test_size = test_size + 5;
    for (size_t i = 0; i < extended_test_size; ++i) {
        bool res = buffer.push(i);
        if (i < test_size) TEST_ASSERT_EQUAL(true, res);
        else
            TEST_ASSERT_EQUAL(false, res);
    }
    for (size_t i = 0; i < test_size; ++i) {
        int v;
        TEST_ASSERT_EQUAL(true, buffer.pop(v));
        TEST_ASSERT_EQUAL(i, v);
    }

    TEST_ASSERT_EQUAL(true, buffer.empty());
    TEST_ASSERT_EQUAL(0, buffer.overrun_space());

    // fill with rollover
    for (size_t i = 0; i < extended_test_size; ++i) {
        buffer.push(i, true);
    }
    TEST_ASSERT_EQUAL(extended_test_size - test_size, buffer.overrun_space());
    for (size_t i = extended_test_size - test_size; i < extended_test_size; ++i) {
        int v;
        TEST_ASSERT_EQUAL(true, buffer.pop(v));
        TEST_ASSERT_EQUAL(i, v);
    }

    // fill buffer back up again
    for (size_t i = 0; i < 9; ++i) {
        buffer.push(i);
    }

    // dropping elements in buffer
    TEST_ASSERT_EQUAL(9, buffer.used_space());
    buffer.drop_first(3);
    TEST_ASSERT_EQUAL(6, buffer.used_space());
    buffer.drop_last(3);
    TEST_ASSERT_EQUAL(3, buffer.used_space());

    for (size_t i = 0; i < 3; ++i) {
        int v;
        buffer.peek_at(v, i);
        TEST_ASSERT_EQUAL(3 + i, v);
    }
    buffer.clear();
    TEST_ASSERT_EQUAL(0, buffer.used_space());
    buffer.drop_last(3);
    buffer.drop_first(3);
    TEST_ASSERT_EQUAL(0, buffer.used_space());
    TEST_ASSERT_EQUAL(buffer.capacity(), buffer.free_space());
}

void ut_ringbuffer_range() {
    const auto test = [](size_t size, bool rollover, std::vector<int> input, std::vector<int> expected) {
        RingBuffer<int> buffer(size);

        if (rollover) {
            TEST_ASSERT_EQUAL(input.size(), buffer.push(input.data(), input.size(), rollover));
        } else {
            TEST_ASSERT_EQUAL(size, buffer.push(input.data(), input.size(), rollover));
        }

        auto res = std::vector<int>(expected.size());
        TEST_ASSERT_EQUAL(expected.size(), buffer.pop(res.data(), res.size()));

        for (size_t i = 0; i < res.size(); ++i) {
            TEST_ASSERT_EQUAL(expected[i], res[i]);
        }
        TEST_ASSERT_EQUAL(true, buffer.empty());
    };

    test(3, true, {1, 2, 3}, {1, 2, 3});
    test(3, false, {1, 2, 3}, {1, 2, 3}); // equals pequals

    test(1, true, {1, 2, 3, 4, 5}, {5});
    test(1, false, {1, 2, 3, 4, 5}, {1});

    // single rollover
    test(3, true, {1, 2, 3, 4, 5}, {3, 4, 5});
    test(3, false, {1, 2, 3, 4, 5}, {1, 2, 3});

    // multiple rollovers
    test(3, false, {1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3});
    test(3, true, {1, 2, 3, 4, 5, 6, 7, 8, 9}, {7, 8, 9});
}

void ut_ringbuffer_rollover() {
    const auto test = [](size_t size, bool rollover, std::vector<int> input, std::vector<int> expected) {
        RingBuffer<int> buffer(size);

        for (size_t i = 0; i < input.size(); ++i) {
            TEST_ASSERT_EQUAL(i < size || rollover, buffer.push(input[i], rollover));
        }
        for (size_t i = 0; i < expected.size(); ++i) {
            int v;
            TEST_ASSERT_EQUAL(true, buffer.pop(v));
            TEST_ASSERT_EQUAL(expected[i], v);
        }
        TEST_ASSERT_EQUAL(true, buffer.empty());
    };

    test(1, true, {1, 2, 3, 4, 5}, {5});
    test(1, false, {1, 2, 3, 4, 5}, {1});

    // single rollover
    test(3, true, {1, 2, 3, 4, 5}, {3, 4, 5});
    test(3, false, {1, 2, 3, 4, 5}, {1, 2, 3});

    // multiple rollovers
    test(3, false, {1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3});
    test(3, true, {1, 2, 3, 4, 5, 6, 7, 8, 9}, {7, 8, 9});
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_ringbuffer_basics);
    RUN_TEST(ut_ringbuffer_range);
    RUN_TEST(ut_ringbuffer_rollover);
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
