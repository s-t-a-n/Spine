// #if defined(ARDUINO) && defined(EMBEDDED)
// #    include <Arduino.h>
// #elif defined(ARDUINO)
// #    include <ArduinoFake.h>
// #endif

#include "spine/eventsystem/eventsystem.hpp"

#include <unity.h>

#include <limits>

using namespace spn::core;

class EventSystemTest : public EventSystem {
public:
    using EventSystem::EventSystem;

    const Pipeline& pipeline() const { return this->_pipeline; }
    const Pool<Event>& store() const { return this->_store; }
};

template<typename T>
class PoolTest : public Pool<T> {
public:
    using Pointer = typename Pool<T>::Pointer;
    using Pool<T>::Pool;

    const Vector<Pointer>& ptr() const { return this->_pointers; }

private:
};

auto sum_of_sp_usecounts = [](const Pool<Event>& pool_orig) {
    //
    const auto& pool = *static_cast<const PoolTest<Event>*>(&pool_orig);
    const auto& vec = pool.ptr();
    size_t usecount_sum = 0;
    for (const auto& t : vec) {
        usecount_sum += t.use_count();
    }
    return usecount_sum;
};

auto all_sp_usecounts_are = [](const Pool<Event>& pool_orig, const size_t usecount) {
    const auto& pool = *static_cast<const PoolTest<Event>*>(&pool_orig);
    return sum_of_sp_usecounts(pool) == pool.ptr().size() * usecount;
};

enum class Events { EventA, EventB, Size };

class TestEventHandlerA : public EventHandler {
public:
    TestEventHandlerA(EventSystem* evsys) : EventHandler(evsys){};
    ~TestEventHandlerA() override = default;

    void handle_event(const Event& event) override {
        switch (static_cast<Events>(event.id())) {
        case Events::EventA: event_handler_ctr++; break;
        case Events::EventB: event_handler_ctr++; break;
        default: break;
        }
    }

    int event_handler_ctr = 0;
};

class TestEventHandlerB : public EventHandler {
public:
    TestEventHandlerB(EventSystem* evsys) : EventHandler(evsys){};
    ~TestEventHandlerB() override = default;

    void handle_event(const Event& event) override {
        switch (static_cast<Events>(event.id())) {
        case Events::EventA: event_handler_ctr++; break;
        case Events::EventB: event_handler_ctr++; break;
        default: break;
        }
    }

    int event_handler_ctr = 0;
};

#if defined(ARDUINO) && !defined(EMBEDDED)
using namespace fakeit;
#endif

void ut_ev_basics() {
#if defined(ARDUINO)
    ARDUINO_MOCK_ALL();
#endif
    const auto events_cap = 128;
    auto sc_cfg = EventSystem::Config{
        .events_count = static_cast<size_t>(Events::Size),
        .events_cap = events_cap,
        .handler_cap = 2,
        .delay_between_ticks = false,
    };
    auto sc = EventSystemTest(sc_cfg);
    TEST_ASSERT_EQUAL(1, all_sp_usecounts_are(sc.store(), 1));

    auto handler = TestEventHandlerA(&sc);
    sc.attach(Events::EventA, &handler);
    TEST_ASSERT_EQUAL(true, handler.event_handler_ctr == 0);

    {
        auto event = sc.event(Events::EventA, time_ms(100), Event::Data());
        TEST_ASSERT_EQUAL(events_cap + 1, sum_of_sp_usecounts(sc.store()));
        sc.trigger(event);
    }

    TEST_ASSERT_EQUAL(true, handler.event_handler_ctr == 1);
    TEST_ASSERT_EQUAL(true, all_sp_usecounts_are(sc.store(), 1));

    auto i = 0;
    for (auto& ev : sc.pipeline().pipe()) {
        TEST_ASSERT_EQUAL(true, i++ == 0);
        TEST_ASSERT_EQUAL(true, ev != nullptr);
        TEST_ASSERT_EQUAL(true, ev->future() > time_ms(0));
        TEST_ASSERT_EQUAL(true, ev->time_until_future() > time_ms(0));
    }

    sc.schedule(sc.event(Events::EventA, time_ms(100), Event::Data()));
    TEST_ASSERT_EQUAL(1, handler.event_handler_ctr);

    // test if event is processed at the right time
#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(50);
#endif
    sc.loop();
    TEST_ASSERT_EQUAL(1, handler.event_handler_ctr);

#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(100);
#else
    HAL::delay(time_ms(100));
#endif
    sc.loop();
    TEST_ASSERT_EQUAL(2, handler.event_handler_ctr);

    // test if seconds properly convert to milliseconds
#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
#endif
    handler.event_handler_ctr = 0;
    {
        auto event = sc.event(Events::EventA, time_s(2), Event::Data());
        sc.schedule(std::move(event));
    }
    // test if event is processed at the right time
#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(1500);
#else
    HAL::delay(time_ms(1500));
#endif
    sc.loop();
    TEST_ASSERT_EQUAL(0, handler.event_handler_ctr);

#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(2000);
#else
    HAL::delay(time_ms(2000));
#endif
    sc.loop();
    TEST_ASSERT_EQUAL(1, handler.event_handler_ctr);
}

void ut_ev_repeat_use() {
#if defined(ARDUINO)
    ARDUINO_MOCK_ALL();
#endif
    const auto event_cap = 1024;

    auto sc_cfg = EventSystem::Config{
        .events_count = static_cast<size_t>(Events::Size),
        .events_cap = event_cap,
        .handler_cap = 2,
        .delay_between_ticks = false,
    };
    auto sc = EventSystemTest(sc_cfg);

    auto handler = TestEventHandlerA(&sc);
    sc.attach(Events::EventA, &handler);
    TEST_ASSERT_EQUAL(true, handler.event_handler_ctr == 0);

    for (int i = 0; i < event_cap; ++i) {
        auto event = sc.event(Events::EventA, time_ms(i), Event::Data());
        TEST_ASSERT_EQUAL(false, event == nullptr);
        sc.schedule(std::move(event));
    }
    TEST_ASSERT_EQUAL(true, sc.event(Events::EventA, time_ms(1), Event::Data()) == nullptr);

#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(event_cap);
#else
    HAL::delay(time_ms(event_cap));
#endif

    sc.loop();
    TEST_ASSERT_EQUAL(event_cap, handler.event_handler_ctr);

#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
#endif
    for (int i = 0; i < event_cap; ++i) {
        auto event = sc.event(Events::EventA, time_ms(i), Event::Data());
        TEST_ASSERT_EQUAL(false, event == nullptr);
        sc.schedule(std::move(event));
    }
    TEST_ASSERT_EQUAL(true, sc.event(Events::EventA, time_ms(1), Event::Data()) == nullptr);

#if defined(ARDUINO) && !defined(EMBEDDED)
    When(Method(ArduinoFake(), millis)).AlwaysReturn(event_cap);
#else
    HAL::delay(time_ms(event_cap));
#endif
    sc.loop();
    TEST_ASSERT_EQUAL(2 * event_cap, handler.event_handler_ctr);

    // check proper roll around of pool
    for (int i = 0; i < event_cap * 10; ++i) {
        auto event = sc.event(Events::EventA, time_ms(i), Event::Data());
        TEST_ASSERT_EQUAL(false, event == nullptr);
        TEST_ASSERT_EQUAL(true, bool(sc.event(Events::EventA, time_ms(i), Event::Data())));
    }
}

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_ev_basics);
    RUN_TEST(ut_ev_repeat_use);
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
