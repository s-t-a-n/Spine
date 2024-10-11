#include <limits.h>
#include <spine/structure/units/si.hpp>
#include <unity.h>

// using spn::core::time::time_ms;
// using spn::core::time::time_s;

namespace {

// Only test time; assume function of other units is equivalent.

void ut_time_basics() {
    // assignment
    auto v = time_ms(100);
    v += time_ms(100);
    TEST_ASSERT_EQUAL(true, v == time_ms(200));
    v -= time_ms(100);
    TEST_ASSERT_EQUAL(true, v == time_ms(100));

    // boolean, within types
    TEST_ASSERT_EQUAL(true, time_ms(1000) == time_ms(1000));
    TEST_ASSERT_EQUAL(true, time_ms(100) != time_ms(1000));
    TEST_ASSERT_EQUAL(true, time_ms(100) < time_ms(1000));
    TEST_ASSERT_EQUAL(true, time_ms(1000) > time_ms(100));
    TEST_ASSERT_EQUAL(true, time_ms(1000) >= time_ms(100));
    TEST_ASSERT_EQUAL(true, time_ms(1000) >= time_ms(1000));
    TEST_ASSERT_EQUAL(true, time_ms(100) <= time_ms(1000));
    TEST_ASSERT_EQUAL(true, time_ms(1000) <= time_ms(1000));

    // boolean, between types
    TEST_ASSERT_EQUAL(true, time_ms(1000) == time_s(1));
    TEST_ASSERT_EQUAL(true, time_ms(1000) != time_s(1000));
    TEST_ASSERT_EQUAL(true, time_ms(1) <= time_s(1000));
    TEST_ASSERT_EQUAL(true, time_ms(1) < time_s(1000));
    TEST_ASSERT_EQUAL(true, time_s(1) < time_ms(2000));
    TEST_ASSERT_EQUAL(true, time_s(2) > time_ms(1000));

    // magnitudes
    TEST_ASSERT_EQUAL(true, time_ms(1000) == time_s(1));
    TEST_ASSERT_EQUAL(true, time_s(60) == time_m(1));
    TEST_ASSERT_EQUAL(true, time_m(60) == time_h(1));
    TEST_ASSERT_EQUAL(true, time_h(24) == time_d(1));

    //
    TEST_ASSERT_EQUAL(true, time_ms(time_s(1)) == time_s(1));
    TEST_ASSERT_EQUAL(true, time_s(time_ms(1000)) == time_ms(1000));
    TEST_ASSERT_EQUAL(true, time_s(time_ms(1000)).raw<int>() == time_s(1).raw<int>());

    //
    TEST_ASSERT_EQUAL(true, time_ms(1000) + time_ms(1000) == time_s(2));
    TEST_ASSERT_EQUAL(true, time_ms(10000) - time_ms(5000) == time_s(5));

    // negatives
    TEST_ASSERT_EQUAL(true, time_ms(-1000) + time_ms(-1000) == time_s(-2));
    TEST_ASSERT_EQUAL(true, time_ms(-1000) + time_ms(2000) == time_s(1));
    TEST_ASSERT_EQUAL(true, time_ms(-1000) - time_ms(2000) == time_s(-3));
    TEST_ASSERT_EQUAL(true, time_ms(-1000) < time_s(1));
    TEST_ASSERT_EQUAL(true, time_s(20) > time_ms(-1000));

    TEST_ASSERT_EQUAL(true, time_m(30) == time_ms(30 * 60 * 1000));

    // test raw
    TEST_ASSERT_EQUAL_INT(60, time_s(time_m(1)).raw<unsigned>());
    TEST_ASSERT_EQUAL_INT(60, time_s(time_m(1)).raw<signed>());
    TEST_ASSERT_EQUAL_FLOAT(60.0, time_s(time_m(1)).raw<float>());
    TEST_ASSERT_EQUAL_INT(60, time_s(time_m(1)).raw<uint8_t>());
    TEST_ASSERT_EQUAL_INT(static_cast<uint32_t>(4294967295 / 60) * 60, time_s(time_m(4294967295 / 60)).raw<uint32_t>());

    // test multiplication/division by arithmetic type
    TEST_ASSERT_EQUAL(true, time_s(time_m(1) * 2).raw<>() == 120);
    TEST_ASSERT_EQUAL(true, time_s(time_m(2) / 2).raw<>() == 60);
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_time_basics);
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
