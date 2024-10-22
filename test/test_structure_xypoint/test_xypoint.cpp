#include <float.h>
#include <limits.h>
#include <math.h>
#include <spine/structure/point.hpp>
#include <unity.h>

using namespace spn::structure;

namespace {

void ut_xypoint_basics() {
    typedef XYPoint<float> vec;

    vec a(FLT_MIN, FLT_MAX);
    vec b(FLT_MAX, FLT_MAX);

    TEST_ASSERT_TRUE(a.x == FLT_MIN && a.y == FLT_MAX);

    TEST_ASSERT_TRUE(b != a);
    TEST_ASSERT_TRUE(!(a == b));

    vec c(b);
    TEST_ASSERT_TRUE(b == c);
    TEST_ASSERT_TRUE(a != c);

    c = a;
    TEST_ASSERT_TRUE(b != c);
    TEST_ASSERT_TRUE(a == c);

    a = b;
    TEST_ASSERT_TRUE(b == a);
    TEST_ASSERT_TRUE(a == b);

    vec d;
    TEST_ASSERT_TRUE(d.x == 0 && d.y == 0);
}

void ut_xypoint_operator_overloads() {
    typedef XYPoint<int> vec;

    vec a(1, 2);
    vec b(4, 5);

    TEST_ASSERT_TRUE(a + b == vec(5, 7));
    TEST_ASSERT_TRUE(a - b == vec(-3, -3));
    TEST_ASSERT_TRUE(a * b == vec(4, 10));
    TEST_ASSERT_TRUE(a / b == vec(1 / 4, 2 / 5));
    TEST_ASSERT_TRUE(a % b == vec(1, 2));
    TEST_ASSERT_TRUE(b % a == vec(0, 1));
}

void ut_xypoint_math() {
    typedef XYPoint<float> vec;

    vec a(1, 2);
    vec b(4, 5);

    vec c;
    c = a;
    c.add(b);
    TEST_ASSERT_TRUE(c == a + b);

    c = a;
    c.subtract(b);
    TEST_ASSERT_TRUE(c == a - b);

    for (float i = -100; i < 100; i++) {
        c = a;
        c.scalar_multiplication(i);
        TEST_ASSERT_TRUE(c == vec(i * a.x, i * a.y));
    }

    c = a;
    float m = c.mag();
    TEST_ASSERT_TRUE(m == sqrtf(5));

    c = a;
    c.normalize();
    TEST_ASSERT_TRUE(c == vec(a.x / m, a.y / m));

    c = a.dot(b);
    TEST_ASSERT_TRUE(c == vec(a.x * b.x, a.y * b.y));
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_xypoint_basics);
    RUN_TEST(ut_xypoint_operator_overloads);
    RUN_TEST(ut_xypoint_math);
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
