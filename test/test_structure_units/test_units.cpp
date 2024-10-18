#include <spine/core/exception.hpp>
#include <spine/structure/units/primitives/compound_unit.hpp>
#include <spine/structure/units/si.hpp>
#include <unity.h>

#include <climits>
#include <cstdio>
#include <memory>
#include <stdexcept>

namespace {

void ut_unit_kernel_time_assignment() {
    auto v = k_time_ms(100);
    v += k_time_ms(100);
    TEST_ASSERT_TRUE(v == k_time_ms(200));
    v -= k_time_ms(100);
    TEST_ASSERT_TRUE(v == k_time_ms(100));
}

void ut_unit_kernel_time_boolean_within_types() {
    TEST_ASSERT_TRUE(k_time_ms(1000) == k_time_ms(1000));
    TEST_ASSERT_TRUE(k_time_ms(100) != k_time_ms(1000));
    TEST_ASSERT_TRUE(k_time_ms(100) < k_time_ms(1000));
    TEST_ASSERT_TRUE(k_time_ms(1000) > k_time_ms(100));
    TEST_ASSERT_TRUE(k_time_ms(1000) >= k_time_ms(100));
    TEST_ASSERT_TRUE(k_time_ms(1000) >= k_time_ms(1000));
    TEST_ASSERT_TRUE(k_time_ms(100) <= k_time_ms(1000));
    TEST_ASSERT_TRUE(k_time_ms(1000) <= k_time_ms(1000));
}

void ut_unit_kernel_time_boolean_between_types() {
    TEST_ASSERT_TRUE(k_time_ms(1000) == k_time_s(1));
    TEST_ASSERT_TRUE(k_time_ms(1000) != k_time_s(1000));
    TEST_ASSERT_TRUE(k_time_ms(1) <= k_time_s(1000));
    TEST_ASSERT_TRUE(k_time_ms(1) < k_time_s(1000));
    TEST_ASSERT_TRUE(k_time_s(1) < k_time_ms(2000));
    TEST_ASSERT_TRUE(k_time_s(2) > k_time_ms(1000));
}

void ut_unit_kernel_time_magnitudes() {
    TEST_ASSERT_TRUE(k_time_ms(1000) == k_time_s(1));
    TEST_ASSERT_TRUE(k_time_s(60) == k_time_m(1));
    TEST_ASSERT_TRUE(k_time_m(60) == k_time_h(1));
    TEST_ASSERT_TRUE(k_time_h(24) == k_time_d(1));
}

void ut_unit_kernel_time_conversions() {
    TEST_ASSERT_TRUE(k_time_ms(k_time_s(1)) == k_time_s(1));
    TEST_ASSERT_TRUE(k_time_s(k_time_ms(1000)) == k_time_ms(1000));
    TEST_ASSERT_EQUAL_INT(k_time_s(1).raw(), k_time_s(k_time_ms(1000)).raw());
    TEST_ASSERT_EQUAL_INT(0, k_time_s(k_time_ms(500)).raw()); // Holds true only for k_time
}

void ut_unit_kernel_time_arithmetic() {
    TEST_ASSERT_TRUE(k_time_ms(1000) + k_time_ms(1000) == k_time_s(2));
    TEST_ASSERT_TRUE(k_time_ms(10000) - k_time_ms(5000) == k_time_s(5));
}

void ut_unit_kernel_time_negatives() {
    TEST_ASSERT_TRUE(k_time_ms(-1000) + k_time_ms(-1000) == k_time_s(-2));
    TEST_ASSERT_TRUE(k_time_ms(-1000) + k_time_ms(2000) == k_time_s(1));
    TEST_ASSERT_TRUE(k_time_ms(-1000) - k_time_ms(2000) == k_time_s(-3));
    TEST_ASSERT_TRUE(k_time_ms(-1000) < k_time_s(1));
    TEST_ASSERT_TRUE(k_time_s(20) > k_time_ms(-1000));
    TEST_ASSERT_TRUE(k_time_m(30) == k_time_ms(30 * 60 * 1000));
}

void ut_unit_kernel_time_raw() {
    TEST_ASSERT_EQUAL_INT(60, k_time_s(k_time_m(1)).raw());
    TEST_ASSERT_TRUE((std::is_same_v<double, decltype(k_time_m(1).raw<double>())>));
}

void ut_unit_kernel_time_multiplication_division() {
    TEST_ASSERT_EQUAL_INT(120, k_time_s(k_time_m(1) * 2).raw());
    TEST_ASSERT_EQUAL_INT(60, k_time_s(k_time_m(2) / 2).raw());
}

void ut_unit_kernel_time_conversion_functions() {
    TEST_ASSERT_EQUAL_INT(2, to_kernel_time(time_ms(1.4)).raw());
    TEST_ASSERT_EQUAL_INT(2, to_kernel_time(time_ms(1.5)).raw());

    TEST_ASSERT_EQUAL_INT(1, to_kernel_time(time_ms(1.6), std::floor).raw());
    TEST_ASSERT_EQUAL_INT(2, to_kernel_time(time_ms(1.4), std::ceil).raw());

    TEST_ASSERT_EQUAL_DOUBLE(2.0, to_real_time(k_time_ms(2)).raw());
}

void ut_unit_kernel_time_printable() {
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%li", k_time_ms(100).raw());
    TEST_ASSERT_EQUAL_STRING("100", buffer);
}

void ut_unit_addition() {
    TEST_ASSERT_EQUAL(true, k_time_ms(1000) + k_time_ms(1000) == k_time_s(2));
    TEST_ASSERT_EQUAL(true, k_time_ms(10000) - k_time_ms(5000) == k_time_s(5));
}

void ut_unit_multiplication() {
    TEST_ASSERT_EQUAL_DOUBLE(6.0, (joule(2) * 3).raw());
    TEST_ASSERT_EQUAL_DOUBLE(0.006, (joule(2) * 0.003).raw());
}

void ut_unit_division() {
    TEST_ASSERT_EQUAL_DOUBLE(2, (joule(6) / 3).raw());
    TEST_ASSERT_EQUAL_DOUBLE(2000.0, (joule(6) / 0.003).raw());
}

void ut_unit_subtraction_assignment() {
    TEST_ASSERT_EQUAL_DOUBLE(-1, (joule(2) -= joule(3)).raw());
    TEST_ASSERT_EQUAL_DOUBLE(1.997, (joule(2) -= joule_m(3)).raw());
}

void ut_unit_addition_assignment() {
    TEST_ASSERT_EQUAL_DOUBLE(5, (joule(2) += joule(3)).raw());
    TEST_ASSERT_EQUAL_DOUBLE(3002, (joule_m(2) += joule(3)).raw());
}

void ut_unit_real_time() {
    TEST_ASSERT_EQUAL_DOUBLE(0.5, time_s(time_ms(500)).raw()); // holds true only for time, not for k_time
}

void ut_unit_real_time_printable() {
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%.2f", time_s(time_ms(100)).raw());
    TEST_ASSERT_EQUAL_STRING("0.10", buffer);
}

void ut_unit_magnitudes() {
    TEST_ASSERT_EQUAL(true, newton(1500) == newton_k(1.5));
    TEST_ASSERT_DOUBLE_WITHIN(1e-15, 1.0, joule_p(joule_G(1e-21)).raw());
    TEST_ASSERT_EQUAL(true, litre(1000) == cubic_metre(1));
}

void ut_unit_temperature_convert() {
    auto t_k = kelvin(100);
    auto t_c = celsius(100);

    TEST_ASSERT_EQUAL(true, convert_to_celsius(t_c) == t_c);
    TEST_ASSERT_EQUAL(true, convert_to_celsius(t_k).raw() == t_k.raw() - 273.15);

    TEST_ASSERT_EQUAL(true, convert_to_kelvin(t_k) == t_k);
    TEST_ASSERT_EQUAL(true, convert_to_kelvin(t_c).raw() == t_c.raw() + 273.15);
}

void ut_unit_exception_handling() {
    using namespace spn::structure::units;
    using namespace spn::core;

    struct EH : public ExceptionHandler {
        void handle_exception(const spn::core::Exception& e) override { throw std::runtime_error(e.what()); }
    };

    auto original_handler = set_machine_exception_handler(std::make_unique<EH>());
    try {
        joule(100) / 0;
        TEST_FAIL_MESSAGE("Expected exception not thrown");
    } catch (const std::runtime_error& e) {
        TEST_ASSERT_EQUAL_STRING("tried to divide by zero", e.what());
    }
    set_machine_exception_handler(std::move(original_handler));
}

void ut_unit_compound() {
    using namespace spnu;
    using mls = CompoundUnit<litre_ml, time_s>;
    using lpm = CompoundUnit<litre, time_m>;

    TEST_ASSERT_EQUAL_DOUBLE(20, mls(litre_ml(600), time_s(30)).raw());
    TEST_ASSERT_EQUAL_DOUBLE(20, mls(litre(0.6), time_s(30)).raw());

    TEST_ASSERT_EQUAL_DOUBLE(120, mls(litre_ml(1200), time_s(10)).raw());
    TEST_ASSERT_EQUAL_DOUBLE(120, mls(litre_ml(1200), time_ms(10000)).raw());

    TEST_ASSERT_EQUAL_DOUBLE(1.2, lpm(litre_ml(600), time_s(30)).raw());
    TEST_ASSERT_EQUAL_DOUBLE(0.6, lpm(litre(0.6), time_s(60)).raw());

    TEST_ASSERT_EQUAL_DOUBLE(1.2, lpm(mls(litre_ml(600), time_s(30))).raw());
}

void ut_unit_compound_addition() {
    using namespace spnu;
    using mls = CompoundUnit<litre_ml, time_s>;
    using lpm = CompoundUnit<litre, time_m>;

    TEST_ASSERT_EQUAL_DOUBLE(1.2, (lpm(litre(0.6), time_s(60)) + lpm(litre(0.6), time_s(60))).raw());
    TEST_ASSERT_EQUAL_DOUBLE(1.2, lpm(mls(litre(0.6), time_s(60)) + lpm(litre(0.6), time_s(60))).raw());
    TEST_ASSERT_EQUAL_DOUBLE(20, mls(lpm(litre(0.6), time_s(60)) + mls(litre(0.6), time_s(60))).raw());
}
void ut_unit_compound_multiplication() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    auto flow_rate = mls(litre_ml(600), time_s(30));
    auto doubled_flow = flow_rate * 2;
    TEST_ASSERT_EQUAL_DOUBLE(40, doubled_flow.raw());
}

void ut_unit_compound_division() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    auto flow_rate = mls(litre_ml(600), time_s(30));
    auto half_flow = flow_rate / 2;
    TEST_ASSERT_EQUAL_DOUBLE(10, half_flow.raw());

    auto flow_rate2 = mls(litre_ml(300), time_s(30));
    double ratio = flow_rate.raw() / flow_rate2.raw();
    TEST_ASSERT_EQUAL_DOUBLE(2, ratio);
}

void ut_unit_compound_subtraction_assignment() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    auto flow_rate1 = mls(litre_ml(600), time_s(30));
    auto flow_rate2 = mls(litre_ml(300), time_s(30));

    flow_rate1 -= flow_rate2;
    TEST_ASSERT_EQUAL_DOUBLE(10, flow_rate1.raw());
}

void ut_unit_compound_addition_assignment() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    auto flow_rate1 = mls(litre_ml(600), time_s(30));
    auto flow_rate2 = mls(litre_ml(400), time_s(20));

    flow_rate1 += flow_rate2;
    TEST_ASSERT_EQUAL_DOUBLE(40, flow_rate1.raw());
}

void ut_unit_compound_comparisons() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    auto flow_rate1 = mls(litre_ml(600), time_s(30));
    auto flow_rate2 = mls(litre_ml(600), time_s(30));
    auto flow_rate3 = mls(litre_ml(300), time_s(30));

    TEST_ASSERT_TRUE(flow_rate1 == flow_rate2);
    TEST_ASSERT_TRUE(flow_rate1 != flow_rate3);
    TEST_ASSERT_TRUE(flow_rate3 < flow_rate1);
    TEST_ASSERT_TRUE(flow_rate1 > flow_rate3);
    TEST_ASSERT_TRUE(flow_rate3 <= flow_rate1);
    TEST_ASSERT_TRUE(flow_rate1 >= flow_rate3);
}

void ut_unit_compound_simplification() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    { // integral
        auto flow_rate = mls(litre_ml(600), time_s(30));
        TEST_ASSERT_EQUAL_DOUBLE(20, flow_rate.raw());
        flow_rate.simplify(1);
        TEST_ASSERT_EQUAL_DOUBLE(20, flow_rate.numerator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(1, flow_rate.denominator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(20, flow_rate.raw());

        flow_rate.simplify(1e1);
        TEST_ASSERT_EQUAL_DOUBLE(2, flow_rate.numerator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(0.1, flow_rate.denominator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(20, flow_rate.raw());
    }

    { // real, single decimal
        auto flow_rate = mls(litre_ml(10.5), time_s(10.5));
        TEST_ASSERT_EQUAL_DOUBLE(1, flow_rate.raw());
        flow_rate.simplify(1e1);
        TEST_ASSERT_EQUAL_DOUBLE(0.1, flow_rate.numerator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(0.1, flow_rate.denominator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(1, flow_rate.raw());
    }

    { // real, two decimals
        auto flow_rate = mls(litre_ml(10.55), time_s(10.55));
        TEST_ASSERT_EQUAL_DOUBLE(1, flow_rate.raw());
        flow_rate.simplify(1e2);
        TEST_ASSERT_EQUAL_DOUBLE(0.01, flow_rate.numerator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(0.01, flow_rate.denominator().raw());
        TEST_ASSERT_EQUAL_DOUBLE(1, flow_rate.raw());
    }
}

void ut_unit_compound_unary_minus() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    auto flow_rate = mls(litre_ml(600), time_s(30));
    auto neg_flow_rate = -flow_rate;
    TEST_ASSERT_EQUAL_DOUBLE(-20, neg_flow_rate.raw());
}

void ut_unit_compound_scalar_operations() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    auto flow_rate = mls(litre_ml(600), time_s(30));
    auto scaled_flow_rate = flow_rate * 2;
    TEST_ASSERT_EQUAL_DOUBLE(40, scaled_flow_rate.raw());

    flow_rate /= 2;
    TEST_ASSERT_EQUAL_DOUBLE(10, flow_rate.raw());
}

void ut_unit_compound_edge_cases() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;

    // zero numerator
    auto zero_flow_rate = mls(litre_ml(0), time_s(30));
    TEST_ASSERT_EQUAL_DOUBLE(0, zero_flow_rate.raw());

    // negative values
    auto negative_flow_rate = mls(litre_ml(-600), time_s(30));
    TEST_ASSERT_EQUAL_DOUBLE(-20, negative_flow_rate.raw());

    // very large values
    auto large_flow_rate = mls(litre_ml(1e6), time_s(1));
    TEST_ASSERT_EQUAL_DOUBLE(1e6, large_flow_rate.raw());
}

void ut_unit_compound_conversions() {
    using namespace spn::structure::units;
    using mls = CompoundUnit<litre_ml, time_s>;
    using lps = CompoundUnit<litre, time_s>;

    auto flow_rate_mls = mls(litre_ml(600), time_m(0.5));
    auto flow_rate_lps = lps(litre(0.6), time_s(30));

    TEST_ASSERT_EQUAL_DOUBLE(flow_rate_mls.raw() / 1000.0, flow_rate_lps.raw());
}

void ut_unit_compound_exception_handling() {
    using namespace spn::structure::units;
    using namespace spn::core;

    using mls = CompoundUnit<litre_ml, time_s>;

    struct EH : public ExceptionHandler {
        void handle_exception(const spn::core::Exception& e) override { throw std::runtime_error(e.what()); }
    };

    auto original_handler = set_machine_exception_handler(std::make_unique<EH>());
    try {
        (void)mls(litre_ml(600), time_s(0)).raw();
        TEST_FAIL_MESSAGE("Expected exception not thrown");
    } catch (const std::runtime_error& e) {
        TEST_ASSERT_EQUAL_STRING("tried to divide by zero", e.what());
    }
    set_machine_exception_handler(std::move(original_handler));
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_unit_kernel_time_assignment);
    RUN_TEST(ut_unit_kernel_time_boolean_within_types);
    RUN_TEST(ut_unit_kernel_time_boolean_between_types);
    RUN_TEST(ut_unit_kernel_time_magnitudes);
    RUN_TEST(ut_unit_kernel_time_conversions);
    RUN_TEST(ut_unit_kernel_time_arithmetic);
    RUN_TEST(ut_unit_kernel_time_negatives);
    RUN_TEST(ut_unit_kernel_time_raw);
    RUN_TEST(ut_unit_kernel_time_multiplication_division);
    RUN_TEST(ut_unit_kernel_time_conversion_functions);
    RUN_TEST(ut_unit_kernel_time_printable);
    RUN_TEST(ut_unit_real_time);
    RUN_TEST(ut_unit_real_time_printable);
    RUN_TEST(ut_unit_addition);
    RUN_TEST(ut_unit_multiplication);
    RUN_TEST(ut_unit_division);
    RUN_TEST(ut_unit_subtraction_assignment);
    RUN_TEST(ut_unit_addition_assignment);
    RUN_TEST(ut_unit_magnitudes);
    RUN_TEST(ut_unit_temperature_convert);
    RUN_TEST(ut_unit_exception_handling);
    RUN_TEST(ut_unit_compound);
    RUN_TEST(ut_unit_compound_addition);
    RUN_TEST(ut_unit_compound_multiplication);
    RUN_TEST(ut_unit_compound_division);
    RUN_TEST(ut_unit_compound_subtraction_assignment);
    RUN_TEST(ut_unit_compound_addition_assignment);
    RUN_TEST(ut_unit_compound_comparisons);
    RUN_TEST(ut_unit_compound_simplification);
    RUN_TEST(ut_unit_compound_unary_minus);
    RUN_TEST(ut_unit_compound_scalar_operations);
    RUN_TEST(ut_unit_compound_edge_cases);
    RUN_TEST(ut_unit_compound_conversions);
    RUN_TEST(ut_unit_compound_exception_handling);
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
