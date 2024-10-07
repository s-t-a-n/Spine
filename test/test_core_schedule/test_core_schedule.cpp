#include "spine/structure/time/schedule.hpp"

#include <limits.h>
#include <unity.h>

namespace {

using spn::core::time::Schedule;

void ut_schedule_basics() {
    // assignment
    const auto duration = time_h(3);

    const auto first = time_h(1);
    const auto second = time_h(1) + duration;
    const auto third = time_h(1) + duration * 2;
    const auto first_value = 27.0;
    const auto second_value = 27.0;
    const auto third_value = 27.0;
    const auto s = Schedule({//
                             Schedule::Block{.start = first, .duration = duration, .value = first_value},
                             Schedule::Block{.start = second, .duration = duration, .value = second_value},
                             Schedule::Block{.start = third, .duration = duration, .value = third_value}});

    const auto test_f = [&s, &duration](time_s block, double last_value, double value, double next_value,
                                        time_s next_block) {
        const auto shortly_before_block = time_s(block) - time_s(1);
        TEST_ASSERT_EQUAL(true, s.start_of_next_block(shortly_before_block) == block);
        TEST_ASSERT_EQUAL(true, s.value_at(shortly_before_block) == last_value);

        const auto shortly_after_start_block = time_s(block) + time_s(1);
        TEST_ASSERT_EQUAL(true, s.start_of_next_block(shortly_after_start_block) == next_block);
        TEST_ASSERT_EQUAL(true, s.value_at(shortly_after_start_block) == value);

        const auto shortly_befor_eend_block = time_s(block) + time_s(duration) - time_s(1);
        TEST_ASSERT_EQUAL(true, s.start_of_next_block(shortly_befor_eend_block) == next_block);
        TEST_ASSERT_EQUAL(true, s.value_at(shortly_befor_eend_block) == value);

        const auto shortly_after_end_block = time_s(block) + time_s(duration) + time_s(1);
        TEST_ASSERT_EQUAL(true, s.value_at(shortly_after_end_block) == next_value);
    };

    test_f(first, 0.0, first_value, second_value, second);
    test_f(second, first_value, second_value, third_value, third);
    test_f(third, second_value, third_value, 0.0, time_h(24));
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_schedule_basics);
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
    run_all_tests();
    return 0;
}
