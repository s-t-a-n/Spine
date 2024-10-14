#include <limits.h>
#include <spine/structure/units/primitives/compound_unit.hpp>
#include <spine/structure/units/si.hpp>
#include <unity.h>

namespace {

void ut_unit_kernel_time() {
    // kernel time uses time_t which on most systems is an int64 type

    // assignment
    auto v = k_time_ms(100);
    v += k_time_ms(100);
    TEST_ASSERT_EQUAL(true, v == k_time_ms(200));
    v -= k_time_ms(100);
    TEST_ASSERT_EQUAL(true, v == k_time_ms(100));

    // boolean, within types
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) == k_time_ms(1000));
    TEST_ASSERT_EQUAL(true, k_time_ms(100) != k_time_ms(1000));
    TEST_ASSERT_EQUAL(true, k_time_ms(100) < k_time_ms(1000));
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) > k_time_ms(100));
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) >= k_time_ms(100));
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) >= k_time_ms(1000));
    TEST_ASSERT_EQUAL(true, k_time_ms(100) <= k_time_ms(1000));
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) <= k_time_ms(1000));

    // boolean, between types
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) == k_time_s(1));
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) != k_time_s(1000));
    TEST_ASSERT_EQUAL(true, k_time_ms(1) <= k_time_s(1000));
    TEST_ASSERT_EQUAL(true, k_time_ms(1) < k_time_s(1000));
    TEST_ASSERT_EQUAL(true, k_time_s(1) < k_time_ms(2000));
    TEST_ASSERT_EQUAL(true, k_time_s(2) > k_time_ms(1000));

    // magnitudes
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) == k_time_s(1));
    TEST_ASSERT_EQUAL(true, k_time_s(60) == k_time_m(1));
    TEST_ASSERT_EQUAL(true, k_time_m(60) == k_time_h(1));
    TEST_ASSERT_EQUAL(true, k_time_h(24) == k_time_d(1));

    // test conversions
    TEST_ASSERT_EQUAL(true, k_time_ms(k_time_s(1)) == k_time_s(1));
    TEST_ASSERT_EQUAL(true, k_time_s(k_time_ms(1000)) == k_time_ms(1000));
    TEST_ASSERT_EQUAL(true, k_time_s(k_time_ms(1000)).raw<int>() == k_time_s(1).raw<int>());
    TEST_ASSERT_EQUAL(true, k_time_s(k_time_ms(500)).raw() == 0); // holds true only for k_time

    // test basic addition
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) + k_time_ms(1000) == k_time_s(2));
    TEST_ASSERT_EQUAL(true, k_time_ms(10000) - k_time_ms(5000) == k_time_s(5));

    // negatives
    TEST_ASSERT_EQUAL(true, k_time_ms(-1000) + k_time_ms(-1000) == k_time_s(-2));
    TEST_ASSERT_EQUAL(true, k_time_ms(-1000) + k_time_ms(2000) == k_time_s(1));
    TEST_ASSERT_EQUAL(true, k_time_ms(-1000) - k_time_ms(2000) == k_time_s(-3));
    TEST_ASSERT_EQUAL(true, k_time_ms(-1000) < k_time_s(1));
    TEST_ASSERT_EQUAL(true, k_time_s(20) > k_time_ms(-1000));

    TEST_ASSERT_EQUAL(true, k_time_m(30) == k_time_ms(30 * 60 * 1000));

    // test raw
    TEST_ASSERT_EQUAL_INT(60, k_time_s(k_time_m(1)).raw<unsigned>());
    TEST_ASSERT_EQUAL_INT(60, k_time_s(k_time_m(1)).raw<signed>());
    TEST_ASSERT_EQUAL_FLOAT(60.0, k_time_s(k_time_m(1)).raw<float>());
    TEST_ASSERT_EQUAL_INT(60, k_time_s(k_time_m(1)).raw<uint8_t>());
    TEST_ASSERT_EQUAL_INT(static_cast<uint32_t>(4294967295 / 60) * 60,
                          k_time_s(k_time_m(4294967295 / 60)).raw<uint32_t>());

    // test multiplication/division by arithmetic type
    TEST_ASSERT_EQUAL(true, k_time_s(k_time_m(1) * 2).raw<>() == 120);
    TEST_ASSERT_EQUAL(true, k_time_s(k_time_m(2) / 2).raw<>() == 60);
}

void ut_unit_fp_time() {
    // normal time, for example: the second denominator in mL/s should be a fp-value for accuracy between whole numbers

    TEST_ASSERT_EQUAL_DOUBLE(0.5, time_s(time_ms(500)).raw()); // holds true only for time, not for k_time
}

void ut_unit_magnitudes() {
    TEST_ASSERT_EQUAL(true, newton(1500) == newton_k(1.5));
    TEST_ASSERT_DOUBLE_WITHIN(1e-15, 1.0, joule_p(joule_G(1e-21)).raw());
}

void ut_unit_temperature_convert() {
    auto t_k = kelvin(100);
    auto t_c = celsius(100);

    TEST_ASSERT_EQUAL(true, convert_to_celsius(t_c) == t_c);
    TEST_ASSERT_EQUAL(true, convert_to_celsius(t_k).raw() == t_k.raw() - 273.15);

    TEST_ASSERT_EQUAL(true, convert_to_kelvin(t_k) == t_k);
    TEST_ASSERT_EQUAL(true, convert_to_kelvin(t_c).raw() == t_c.raw() + 273.15);
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_unit_kernel_time);
    RUN_TEST(ut_unit_fp_time);
    RUN_TEST(ut_unit_magnitudes);
    RUN_TEST(ut_unit_temperature_convert);
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
