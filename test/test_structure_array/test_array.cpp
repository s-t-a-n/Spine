#include <limits.h>
#include <spine/structure/array.hpp>
#include <unity.h>

using namespace spn::structure;

namespace {

void ut_array_basics() {
    const int max_size = 128;
    Array<int> Arr(max_size);

    for (int i = 0; i < max_size; i++) {
        Arr[i] = i;
        TEST_ASSERT_EQUAL(Arr[i], i);
    }
}

void ut_array_underlying_data() {
    const int max_size = 128;
    Array<int> Arr(max_size);

    for (int i = 0; i < max_size; i++) {
        Arr[i] = i;
    }

    int* int_ptr = Arr.data();
    for (int i = 0; i < max_size; i++) {
        TEST_ASSERT_EQUAL(int_ptr[i], i);
    }
}

void ut_array_external_store() {
    const int max_size = 128;
    int store[max_size];

    Array<int> Arr(store);

    for (int i = 0; i < max_size; i++) {
        Arr[i] = i;
    }
}

void ut_array_set_external_store() {
    const int max_size = 128;
    int store[max_size];

    Array<int> Arr(store);

    for (int i = 0; i < max_size; i++) {
        Arr[i] = i;
    }
}

void ut_array_iteration() {
    const int max_size = 128;
    Array<int> Arr(max_size);

    int ctr = 0;
    for (int& i : Arr) {
        i = ctr++;
        TEST_ASSERT_EQUAL(i, Arr[i]);
        TEST_ASSERT_EQUAL(i, Arr.at(i));
    }
    TEST_ASSERT_EQUAL(max_size, ctr);

    Array<int>::iterator i;
    ctr = 0;
    for (i = Arr.begin(); i != Arr.end(); i++) {
        TEST_ASSERT_EQUAL(*i, Arr[ctr]);
        TEST_ASSERT_EQUAL(*i, Arr.at(ctr));
        TEST_ASSERT_EQUAL(*i, ctr);
        ctr++;
    }
    TEST_ASSERT_EQUAL(max_size, ctr);
}

void ut_array_reverse_iteration() {
    const int max_size = 128;
    Array<int> Arr(max_size);

    int ctr = 0;
    for (int& i : Arr) {
        i = ctr++;
        TEST_ASSERT_EQUAL(i, Arr[i]);
        TEST_ASSERT_EQUAL(i, Arr.at(i));
    }
    TEST_ASSERT_EQUAL(max_size, ctr);

    Array<int>::reverse_iterator i;
    ctr = max_size - 1;
    for (i = Arr.rbegin(); i != Arr.rend(); i++) {
        TEST_ASSERT_EQUAL(*i, Arr[ctr]);
        TEST_ASSERT_EQUAL(*i, Arr.at(ctr));
        ctr--;
    }
    TEST_ASSERT_EQUAL(-1, ctr);
}

void ut_array_fill() {
    const int max_size = 128;
    Array<int> Arr(max_size);
    Arr.fill(0);

    for (int i = 0; i < max_size; i++) {
        TEST_ASSERT_EQUAL(Arr[i], 0);
    }

    Arr.fill(INT_MIN);

    for (int i = 0; i < max_size; i++) {
        TEST_ASSERT_EQUAL(Arr[i], INT_MIN);
    }
}

void ut_array_swap() {
    const int max_size = 128;
    Array<int> Arr_A(max_size);
    Array<int> Arr_B(max_size);
    Arr_A.fill('A');
    Arr_B.fill('B');

    Arr_A.swap(Arr_B);
    TEST_ASSERT_EQUAL(Arr_B.back(), 'A');
    TEST_ASSERT_EQUAL(Arr_A.back(), 'B');
}

void ut_array_getters() {
    const int max_size = 128;
    Array<int> Arr(max_size);

    TEST_ASSERT_EQUAL(max_size, Arr.max_size());
    TEST_ASSERT_EQUAL(max_size, Arr.size());
    TEST_ASSERT_EQUAL(false, Arr.empty());

    Array<int> empty_Arr(0);
    TEST_ASSERT_EQUAL(0, empty_Arr.max_size());
    TEST_ASSERT_EQUAL(0, empty_Arr.size());
    TEST_ASSERT_EQUAL(true, empty_Arr.empty());
}

void ut_array_front_back() {
    const int max_size = 128;
    Array<int> Arr(max_size);

    Arr.front() = 1;
    Arr.back() = 2;
    TEST_ASSERT_EQUAL(1, Arr[0]);
    TEST_ASSERT_EQUAL(2, Arr[max_size - 1]);
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_array_basics);
    RUN_TEST(ut_array_underlying_data);
    RUN_TEST(ut_array_external_store);
    RUN_TEST(ut_array_set_external_store);
    RUN_TEST(ut_array_iteration);
    RUN_TEST(ut_array_reverse_iteration);
    RUN_TEST(ut_array_fill);
    RUN_TEST(ut_array_swap);
    RUN_TEST(ut_array_getters);
    RUN_TEST(ut_array_front_back);
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
