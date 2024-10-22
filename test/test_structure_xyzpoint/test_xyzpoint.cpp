#include <float.h>
#include <limits.h>
#include <math.h>
#include <spine/structure/point.hpp>
#include <unity.h>

using namespace spn::structure;

namespace {

void ut_xyzpoint_basics() {
    typedef XYZPoint<float> vec;

    vec a(FLT_MIN, 0, FLT_MAX);
    vec b(FLT_MAX, FLT_MAX, FLT_MAX);

    TEST_ASSERT_TRUE(a.x == FLT_MIN && a.y == 0 && a.z == FLT_MAX);

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
    TEST_ASSERT_TRUE(d.x == 0 && d.y == 0 && d.z == 0);
}

void ut_xyzpoint_operator_overloads() {
    typedef XYZPoint<int> vec;

    vec a(1, 2, 3);
    vec b(4, 5, 6);

    TEST_ASSERT_TRUE(a + b == vec(5, 7, 9));
    TEST_ASSERT_TRUE(a - b == vec(-3, -3, -3));
    TEST_ASSERT_TRUE(a * b == vec(4, 10, 18));
    TEST_ASSERT_TRUE(a / b == vec(1 / 4, 2 / 5, 3 / 6));
    TEST_ASSERT_TRUE(a % b == vec(1, 2, 3));
    TEST_ASSERT_TRUE(b % a == vec(0, 1, 0));
}

void ut_xyzpoint_math() {
    typedef XYZPoint<float> vec;

    vec a(1, 2, 3);
    vec b(4, 5, 6);

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
        TEST_ASSERT_TRUE(c == vec(i * a.x, i * a.y, i * a.z));
    }

    c = a;
    float m = c.mag();
    TEST_ASSERT_TRUE(m == sqrtf(14));

    c = a;
    c.normalize();
    TEST_ASSERT_TRUE(c == vec(a.x / m, a.y / m, a.z / m));

    c = a.dot(b);
    TEST_ASSERT_TRUE(c == vec(a.x * b.x, a.y * b.y, a.z * b.z));
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_xyzpoint_basics);
    RUN_TEST(ut_xyzpoint_operator_overloads);
    RUN_TEST(ut_xyzpoint_math);
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
