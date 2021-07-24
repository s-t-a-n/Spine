#include <limits.h>
#include <spine/structure/pool.hpp>
#include <spine/structure/vector.hpp>
#include <unity.h>

#include <cstdint>
#include <cstdlib>

using namespace spn::structure;

namespace {

template<typename T>
class PoolTest : public Pool<T> {
public:
    using Pointer = typename Pool<T>::Pointer;
    using Pool<T>::Pool;

    const Vector<Pointer>& ptr() const { return this->_pointers; }

private:
};

struct Test {
    int v;
};

constexpr auto TEST_LENGTH = 128; // completely arbitrary

auto sum_of_sp_usecounts = [](const PoolTest<Test>& pool) {
    //
    const auto& vec = pool.ptr();
    size_t usecount_sum = 0;
    for (const auto& t : vec) {
        usecount_sum += t.use_count();
    }
    return usecount_sum;
};

auto all_sp_usecounts_are = [](const PoolTest<Test>& pool, const size_t usecount) {
    return sum_of_sp_usecounts(pool) == pool.ptr().size() * usecount;
};

void ut_pool_allocation_basics() {
    auto test_f = [](uint16_t size) {
        PoolTest<Test> p(size);

        for (int i = 0; i < size; ++i) {
            p.populate(Test{.v = i});
        }
        TEST_ASSERT_EQUAL(true, p.is_fully_populated());
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 1));

        Vector<std::shared_ptr<Test>> results(size);

        for (int i = 0; i < size; ++i) {
            auto r_s = p.acquire();
            TEST_ASSERT_EQUAL(true, bool(r_s));
            TEST_ASSERT_EQUAL_INT16(i, r_s->v);
            results.emplace_back(std::move(r_s));
        }
        TEST_ASSERT_EQUAL(results.max_size(), results.size());
        TEST_ASSERT_EQUAL(2 * size, sum_of_sp_usecounts(p));
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 2));
        TEST_ASSERT_EQUAL(false, bool(p.acquire()));
        TEST_ASSERT_EQUAL(false, bool(p.can_acquire()));

        // verify that a slice becomes available when the shared ptr reaches a single reference to self
        { auto r = results.pop_back(); }
        TEST_ASSERT_EQUAL(true, bool(p.acquire()));
        TEST_ASSERT_EQUAL(true, bool(p.can_acquire()));
        TEST_ASSERT_EQUAL(results.size() * 2 + 1, sum_of_sp_usecounts(p));

        // verify depopulate function and verify it's order of release
        for (int i = 0; i < size; ++i) {
            auto r_s = p.depopulate();
            TEST_ASSERT_EQUAL(true, bool(r_s));
            TEST_ASSERT_EQUAL_INT16(i, r_s->v);
        }
        TEST_ASSERT_EQUAL(false, bool(p.can_acquire()));
        TEST_ASSERT_EQUAL(false, bool(p.acquire()));

        results.clear();
    };
    // test_f(10);

    for (int i = 1; i < TEST_LENGTH; ++i) {
        test_f(i);
    }
}

void ut_pool_repeat_use() {
    auto test_f = [](uint16_t size) {
        PoolTest<Test> p(size);
        for (int i = 0; i < size; ++i) {
            p.populate(Test{.v = i});
        }
        TEST_ASSERT_EQUAL(true, p.is_fully_populated());
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 1));

        Vector<std::shared_ptr<Test>> results(size);

        for (int i = 0; i < size; ++i) {
            auto r_s = p.acquire();
            TEST_ASSERT_EQUAL(true, r_s != nullptr);

            TEST_ASSERT_EQUAL(i, r_s.get()->v);
            results.push_back(r_s);
        }
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 2));
        results.clear();
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 1));

        for (int i = 0; i < size + 1; ++i) {
            auto r_s = p.acquire();
            if (i == size) {
                TEST_ASSERT_EQUAL(true, r_s == nullptr);
                break;
            }
            TEST_ASSERT_EQUAL(true, r_s != nullptr);
            results.push_back(r_s);
        }
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 2));
        TEST_ASSERT_EQUAL(false, bool(p.acquire()));
        TEST_ASSERT_EQUAL(false, bool(p.can_acquire()));
        results.clear();
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 1));

        TEST_ASSERT_EQUAL(true, bool(p.acquire()));
        TEST_ASSERT_EQUAL(true, bool(p.can_acquire()));
        for (int i = 0; i < size; ++i) {
            auto r_s = p.acquire();
            TEST_ASSERT_EQUAL(true, r_s != nullptr);
            results.push_back(r_s);
        }
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 2));
        TEST_ASSERT_EQUAL(false, bool(p.acquire()));
        TEST_ASSERT_EQUAL(false, bool(p.can_acquire()));
        results.clear();
        TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(p, 1));

        for (int i = 0; i < size; ++i) {
            auto r_s = p.depopulate();
            TEST_ASSERT_EQUAL(true, r_s != nullptr);
            TEST_ASSERT_EQUAL(i, r_s.get()->v);
        }
    };

    for (int i = 1; i < TEST_LENGTH; ++i) {
        test_f(i);
    }
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_pool_allocation_basics);
    RUN_TEST(ut_pool_repeat_use);
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
