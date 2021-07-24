#include "spine/platform/hal.hpp"
#include "spine/structure/vector.hpp"

#include <limits.h>
#include <unity.h>

using namespace spn::structure;

namespace {

void ut_vector_basics() {
    const int max_size = 128;
    Vector<int> vec(max_size);

    // test basic allocation and iteration
    for (size_t i = 0; i < max_size; i++) {
        vec[i] = i;
        TEST_ASSERT_EQUAL(vec[i], i);
    }
}

void ut_vector_underlying_data() {
    const int max_size = 128;
    Vector<int> vec(max_size);

    for (size_t i = 0; i < max_size; i++) {
        vec[i] = i;
    }

    int* int_ptr = vec.data();
    for (size_t i = 0; i < max_size; i++) {
        TEST_ASSERT_EQUAL(int_ptr[i], i);
    }
}

void ut_vector_external_store() {
    const int max_size = 128;
    int store[max_size];

    Vector<int> vec(store);

    for (size_t i = 0; i < max_size; i++) {
        vec[i] = i;
    }
}

void ut_vector_set_external_store() {
    // const int max_size = 128;
    // int store[max_size];
    //
    // Vector<int> vec({});
    // vec.setBackingStore(store);
    //
    // for (size_t i = 0; i < max_size; i++) {
    //     vec[i] = i;
    // }
}

void ut_vector_iteration() {
    const int max_size = 128;
    Vector<int> vec(max_size);
    TEST_ASSERT_TRUE(vec.size() == 0);

    // check that empty container doesnt iterate
    TEST_ASSERT_EQUAL(true, vec.begin() == vec.end());
    auto idx = 0;
    for (auto v : vec) {
        idx++;
    }
    TEST_ASSERT_EQUAL(0, idx);

    vec.fill('A');
    TEST_ASSERT_TRUE(vec.size() == max_size);

    idx = 0;
    for (int& i : vec) {
        i = idx++;
        TEST_ASSERT_EQUAL(i, vec[i]);
        TEST_ASSERT_EQUAL(i, vec.at(i));
    }
    TEST_ASSERT_EQUAL(vec.size(), idx);

    Vector<int>::iterator i;
    idx = 0;
    for (i = vec.begin(); i != vec.end(); i++) {
        TEST_ASSERT_EQUAL(*i, vec[idx]);
        TEST_ASSERT_EQUAL(*i, vec.at(idx));
        TEST_ASSERT_EQUAL(*i, idx);
        idx++;
    }
    TEST_ASSERT_EQUAL(vec.size(), idx);
}

void ut_vector_reverse_iteration() {
    const int max_size = 128;
    Vector<int> vec(max_size);
    TEST_ASSERT_TRUE(vec.size() == 0);

    // check that empty container doesnt iterate
    auto idx = 0;
    for (Vector<int>::reverse_iterator i = vec.rbegin(); i != vec.rend(); i++) {
        idx++;
    }
    TEST_ASSERT_EQUAL(0, idx);

    vec.fill('A');
    TEST_ASSERT_TRUE(vec.size() == max_size);

    idx = 0;
    for (int& i : vec) {
        i = idx++;
        TEST_ASSERT_EQUAL(i, vec[i]);
        TEST_ASSERT_EQUAL(i, vec.at(i));
    }
    TEST_ASSERT_EQUAL(vec.size(), idx);

    idx = vec.size() - 1;
    for (auto i = vec.rbegin(); i != vec.rend(); i++) {
        TEST_ASSERT_EQUAL(*i, vec[idx]);
        TEST_ASSERT_EQUAL(*i, vec.at(idx));
        idx--;
    }
    TEST_ASSERT_EQUAL((size_t)-1, idx);
}

void ut_vector_shifted_iteration() {
    int i;

    // test forward iteration when element is removed at front
    Vector<int> vec = {1, 2, 3};
    TEST_ASSERT_EQUAL(3, vec.size());
    TEST_ASSERT_EQUAL(3, vec.max_size());
    TEST_ASSERT_EQUAL(1, vec.pop_front());
    TEST_ASSERT_EQUAL(2, vec.size());
    i = 2;
    for (auto v : vec) {
        TEST_ASSERT_EQUAL(i++, v);
    }

    // test forward iteration when element is removed at back
    vec = {1, 2, 3};
    TEST_ASSERT_EQUAL(3, vec.pop_back());
    i = 1;
    for (auto v : vec) {
        TEST_ASSERT_EQUAL(i++, v);
    }

    // test reverse iteration when element is removed at front
    vec = {1, 2, 3};
    [[maybe_unused]] auto _ = vec.pop_front();
    i = 3;
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        TEST_ASSERT_EQUAL(i--, *it);
    }

    // test reverse iteration when element is removed at back
    vec = {1, 2, 3};
    _ = vec.pop_back();
    i = 2;
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        TEST_ASSERT_EQUAL(i--, *it);
    }
}

void ut_vector_fill() {
    const int max_size = 128;
    Vector<int> vec(max_size);
    vec.fill(0);

    for (size_t i = 0; i < max_size; i++) {
        TEST_ASSERT_EQUAL(vec[i], 0);
    }

    vec.fill(INT_MIN);

    for (size_t i = 0; i < max_size; i++) {
        TEST_ASSERT_EQUAL(vec[i], INT_MIN);
    }
    TEST_ASSERT_EQUAL(vec.size(), max_size);
}

void ut_vector_swap() {
    const int max_size = 128;
    Vector<int> vec_A(max_size);
    Vector<int> vec_B(max_size);
    vec_A.fill('A');
    vec_B.fill('B');

    vec_A.swap(vec_B);
    TEST_ASSERT_EQUAL(vec_B.back(), 'A');
    TEST_ASSERT_EQUAL(vec_A.back(), 'B');
}

void ut_vector_getters() {
    const int max_size = 128;
    Vector<int> vec(max_size);

    TEST_ASSERT_EQUAL(max_size, vec.max_size());
    TEST_ASSERT_EQUAL(0, vec.size());
    TEST_ASSERT_EQUAL(true, vec.empty());

    Vector<int> empty_vec({});
    TEST_ASSERT_EQUAL(0, empty_vec.max_size());
    TEST_ASSERT_EQUAL(0, empty_vec.size());
    TEST_ASSERT_EQUAL(true, empty_vec.empty());
}

void ut_vector_pushback() {
    const int max_size = 128;
    Vector<int> vec(max_size);
    TEST_ASSERT_EQUAL(max_size, vec.max_size());

    for (int i = 0; i < max_size + 20; i++)
        vec.push_back(i);
    TEST_ASSERT_EQUAL(max_size, vec.size());

    int ctr = 0;
    for (int& i : vec)
        TEST_ASSERT_EQUAL(ctr++, i);
    TEST_ASSERT_EQUAL(ctr, max_size);
}

void ut_vector_front_back() {
    const int max_size = 128;
    Vector<int> vec(max_size);

    vec.front() = 1;
    TEST_ASSERT_EQUAL(1, vec[0]);
    vec.back() = 2;
    TEST_ASSERT_EQUAL(2, vec[0]);

    vec.push_back(25);
    vec.push_back(25);
    vec.push_back(25);
    vec.front() = 1;
    vec.back() = 2;
    TEST_ASSERT_EQUAL(1, vec[0]);
    TEST_ASSERT_EQUAL(25, vec[1]);
    TEST_ASSERT_EQUAL(2, vec[2]);
}

void ut_vector_pop_front() {
    const int max_size = 128;
    Vector<int> vec(max_size);

    for (int i = 0; i < max_size; i++) {
        vec.push_back(i);
    }

    TEST_ASSERT_EQUAL(0, vec.pop_front());
    TEST_ASSERT_EQUAL(max_size - 1, vec.pop_back());

    int idx = 1;
    for (auto i : vec) {
        TEST_ASSERT_EQUAL(idx++, i);
    }

    const int max_size2 = 4;
    Vector<int> vec2(max_size2);
    vec2.push_front(1);
    TEST_ASSERT_EQUAL(1, vec2.size());
    vec2.push_front(2);
    TEST_ASSERT_EQUAL(2, vec2.size());
    TEST_ASSERT_EQUAL(1, vec2.pop_back());
    TEST_ASSERT_EQUAL(2, vec2.pop_back());
    TEST_ASSERT_EQUAL(0, vec2.size());
    vec2.push_front(3);
    vec2.push_front(2);
    vec2.push_back(4);
    vec2.push_front(1);
    TEST_ASSERT_EQUAL(4, vec2.size());
    TEST_ASSERT_EQUAL(4, vec2.pop_back());
    TEST_ASSERT_EQUAL(1, vec2.pop_front());
    TEST_ASSERT_EQUAL(2, vec2.size());
    TEST_ASSERT_EQUAL(3, vec2.pop_back());
    TEST_ASSERT_EQUAL(2, vec2.pop_front());
    TEST_ASSERT_EQUAL(0, vec2.size());
}

void ut_vector_insert() {
    const int max_size = 6;

    auto test_insert_at = [max_size](size_t idx, size_t number, const std::initializer_list<int>&& source,
                                     const std::initializer_list<int>&& expected) {
        Vector<int> vec(max_size);
        for (auto v : source) {
            vec.push_back(v);
        }
        vec.insert(idx, number);
        for (auto v : expected) {
            TEST_ASSERT_EQUAL_INT(v, vec.pop_front());
        }
    };

    test_insert_at(0, 42, {1, 2, 3, 4, 5}, {42, 1, 2, 3, 4, 5});
    test_insert_at(1, 42, {1, 2, 3, 4, 5}, {1, 42, 2, 3, 4, 5});
    test_insert_at(2, 42, {1, 2, 3, 4, 5}, {1, 2, 42, 3, 4, 5});
    test_insert_at(3, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 42, 4, 5});
    test_insert_at(4, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 42, 5});
    test_insert_at(5, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5, 42});
    test_insert_at(6, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5, 42});
}

void ut_vector_remove() {
    const int max_size = 6;

    auto test_remove_at = [max_size](size_t idx, const std::initializer_list<int>&& source,
                                     const std::initializer_list<int>&& expected) {
        Vector<int> vec(max_size);
        for (auto v : source) {
            vec.push_back(v);
        }
        vec.remove(idx);
        for (auto v : expected) {
            TEST_ASSERT_EQUAL_INT(v, vec.pop_front());
        }
    };

    test_remove_at(0, {1, 2, 3, 4, 5}, {2, 3, 4, 5});
    test_remove_at(1, {1, 2, 3, 4, 5}, {1, 3, 4, 5});
    test_remove_at(2, {1, 2, 3, 4, 5}, {1, 2, 4, 5});
    test_remove_at(3, {1, 2, 3, 4, 5}, {1, 2, 3, 5});
    test_remove_at(4, {1, 2, 3, 4, 5}, {1, 2, 3, 4});
    test_remove_at(5, {1, 2, 3, 4, 5}, {1, 2, 3, 4});
}

void ut_vector_insert_complex() {
    const int max_size = 6;

    auto test_insert_at = [max_size](size_t idx, size_t number, const std::initializer_list<int>&& source,
                                     const std::initializer_list<int>&& expected) {
        Vector<std::shared_ptr<int>> vec(max_size);
        TEST_ASSERT(sizeof(std::shared_ptr<int>) == sizeof(std::shared_ptr<int>));

        for (auto v : source) {
            vec.emplace_back(std::make_shared<int>(v));
        }
        vec.insert(idx, std::make_shared<int>(number));
        for (auto v : expected) {
            auto elem =
                vec.pop_front(); // todo: inlining this in the next line causes the shared_ptr not to be cleared, why?
            TEST_ASSERT_EQUAL_INT(v, *elem.get());
        }
    };

    test_insert_at(0, 42, {1, 2, 3, 4, 5}, {42, 1, 2, 3, 4, 5});
    test_insert_at(1, 42, {1, 2, 3, 4, 5}, {1, 42, 2, 3, 4, 5});
    test_insert_at(2, 42, {1, 2, 3, 4, 5}, {1, 2, 42, 3, 4, 5});
    test_insert_at(3, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 42, 4, 5});
    test_insert_at(4, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 42, 5});
    test_insert_at(5, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5, 42});
}

void ut_vector_insert_complex_random() {
    const int max_size = 6;

    auto test_insert_at = [max_size](size_t idx, size_t number, const std::initializer_list<int>&& source,
                                     const std::initializer_list<int>&& expected) {
        Vector<std::shared_ptr<int>> vec(max_size);
        TEST_ASSERT(sizeof(std::shared_ptr<int>) == sizeof(std::shared_ptr<int>));

        for (auto v : source) {
            vec.emplace_back(std::make_shared<int>(v));
        }
        for (auto& v : vec) {
            TEST_ASSERT_EQUAL(1, v.use_count());
        }

        vec.insert(idx, std::make_shared<int>(number));

        for (auto& v : vec) {
            TEST_ASSERT_EQUAL(1, v.use_count());
        }
        for (auto v : expected) {
            auto elem =
                vec.pop_front(); // todo: inlining this in the next line causes the shared_ptr not to be cleared, why?
            TEST_ASSERT_EQUAL_INT(v, *elem.get());
        }
    };

    test_insert_at(0, 42, {1, 2, 3, 4, 5}, {42, 1, 2, 3, 4, 5});
    test_insert_at(1, 42, {1, 2, 3, 4, 5}, {1, 42, 2, 3, 4, 5});
    test_insert_at(2, 42, {1, 2, 3, 4, 5}, {1, 2, 42, 3, 4, 5});
    test_insert_at(3, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 42, 4, 5});
    test_insert_at(4, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 42, 5});
    test_insert_at(5, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5, 42});
    test_insert_at(6, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5, 42});
    test_insert_at(7, 42, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5, 42});
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_vector_basics);
    RUN_TEST(ut_vector_underlying_data);
    RUN_TEST(ut_vector_external_store);
    RUN_TEST(ut_vector_set_external_store);
    RUN_TEST(ut_vector_iteration);
    RUN_TEST(ut_vector_reverse_iteration);
    RUN_TEST(ut_vector_shifted_iteration);
    RUN_TEST(ut_vector_fill);
    RUN_TEST(ut_vector_swap);
    RUN_TEST(ut_vector_getters);
    RUN_TEST(ut_vector_pushback);
    RUN_TEST(ut_vector_front_back);
    RUN_TEST(ut_vector_pop_front);
    RUN_TEST(ut_vector_insert);
    RUN_TEST(ut_vector_remove);
    RUN_TEST(ut_vector_insert_complex);
    RUN_TEST(ut_vector_insert_complex_random);
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
