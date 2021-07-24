#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/standard.hpp"
#include "spine/core/time.hpp"
#include "spine/core/timers.hpp"
#include "spine/eventsystem/pipeline.hpp"
#include "spine/structure/array.hpp"
#include "spine/structure/pool.hpp"
#include "spine/structure/vector.hpp"

// #include <AH/Math/MinMaxFix.hpp>

#include <string>
#include <variant>

namespace spn::core {

using spn::eventsystem::Future;
using spn::eventsystem::Pipeline;
using spn::structure::Array;
using spn::structure::Pool;
using spn::structure::Vector;

// forward declarations
class EventSystem;
// class EventSystemController;
class EventStore;

class Event : public Future {
public:
    using Id = size_t;

public:
    // a crude data object meant solely to convey a single value known by the sender and receiver.
    class Data {
    public:
        Data() = default;
        explicit Data(time_ms time) : _value(time) {}
        explicit Data(time_s time) : _value(time) {}
        explicit Data(double value) : _value(value) {}
        explicit Data(uint32_t value) : _value(value) {}

        double value() const {
            assert(_value);
            if (!_value) return {};
            assert(std::holds_alternative<double>(*_value));
            return std::get<double>(*_value);
        }
        uint32_t unsigned_value() const {
            assert(_value);
            if (!_value) return {};
            assert(std::holds_alternative<uint32_t>(*_value));
            return std::get<uint32_t>(*_value);
        }
        time_ms ms() const {
            assert(_value);
            if (!_value) return {};
            assert(std::holds_alternative<time_ms>(*_value));
            return std::get<time_ms>(*_value);
        }
        time_s s() const {
            assert(_value);
            if (!_value) return {};
            assert(std::holds_alternative<time_s>(*_value));
            return std::get<time_s>(*_value);
        }

        bool has_value() const { return bool(_value); }

    private:
        using Value = std::variant<double, uint32_t, time_s, time_ms>;
        std::optional<Value> _value;
    };

public:
    Id id() const { return _id; }
    const Data& data() const { return _data; }

    // protected:
    Event() = default;

private:
    Id _id;
    Data _data;
    // Tracker _tracker;

    friend EventStore;
    friend EventSystem;
    //    friend EventSystemController;
};

class EventHandler {
public:
    EventHandler(EventSystem* evsys) : _evsys(evsys) {}

    virtual void handle_event(const Event& event) { assert(!"Virtual base function called"); };
    virtual ~EventHandler() = default;
    void attach_event_system(EventSystem* evsys) {
        assert(_evsys == nullptr);
        assert(evsys != nullptr);
        _evsys = evsys;
    }

protected:
    EventSystem* evsys() {
        assert(_evsys);
        return _evsys;
    };

private:
    EventSystem* _evsys = nullptr;
};

class EventSystem {
public:
    struct Config {
        size_t events_count; // how many events exist
        size_t events_cap; // maximal possible events to be processed
        size_t handler_cap; // maximal amount of handlers per event
        bool delay_between_ticks;
        time_us min_delay_between_ticks = time_ms(100);
        time_us max_delay_between_ticks = time_ms(1000);
    };

public:
    using EventHandlerMap = std::unique_ptr<Vector<EventHandler*>>;

    EventSystem(const Config& cfg) //
        : _cfg(cfg), //
          _map(Array<EventHandlerMap>(cfg.events_count)), //
          _pipeline(Pipeline(cfg.events_cap)), //
          _store({cfg.events_cap}) {
        for (size_t i = 0; i < _cfg.events_count; i++) {
            _map[i] = std::make_unique<Vector<EventHandler*>>(_cfg.handler_cap);
        }

        for (size_t i = 0; i < _cfg.events_cap; i++) {
            _store.populate(std::make_shared<Event>());
        }
    }

    ~EventSystem() {
        DBG("Destroying eventsystem");
        for (size_t i = 0; i < _map.size(); i++) {
            _map[i].reset();
        }
        for (size_t i = 0; i < _store.size(); ++i) {
            auto ev = _store.depopulate();
            ev.reset();
        }
        assert(_store.size() == 0);
    }

public:
    std::string pipeline_as_string() const { return _pipeline.to_string(); }

    template<typename T>
    void attach(T id, EventHandler* handler) {
        auto idx = static_cast<Event::Id>(id);
        assert(handler != nullptr);
        assert(idx < _map.size());
        assert(_map[idx]->size() < _cfg.handler_cap);
        _map[idx]->push_back(handler);
    };

    void detach(EventHandler& handler);

    // external users may wish to directly trigger an event they have laying around
    void trigger(const std::shared_ptr<Event>& event) {
        assert(event);
        trigger(*event.get());
    }

    void trigger(const Event& event) {
        const auto id = event.id();
        assert(id < _map.size());
        assert(_map[id] != nullptr);
        assert(!_map[id]->empty());
        for (auto handler : *_map[id]) {
            handler->handle_event(event);
        }
    };

    template<typename IdType>
    void trigger(IdType id, const Event::Data& data = {}) {
        trigger(event(id, time_ms(0)));
    }

    template<typename IdType>
    std::shared_ptr<Event> event(IdType id, const time_ms& time_from_now, const Event::Data& data = {}) {
        auto event = _store.acquire();
        assert(event); // try to catch gracefully here for use in nested calls
        if (!event) return nullptr;

        event->_id = static_cast<Event::Id>(id);
        event->_data = data;
        event->_time_from_now = time_from_now;
        return event;
    }

    template<typename IdType>
    void schedule(IdType id, const time_ms& time_from_now, const Event::Data& data = {}) {
        schedule(event(id, time_from_now, data));
    }

    void schedule(std::shared_ptr<Event>&& event) {
        // DBGF("Scheduling event %u seconds from now", time_s(event.get()->_time_from_now).raw<unsigned>());
        _pipeline.push(std::move(event));
    }

    void loop() {
        // int i = 0;
        // for (auto& f : _pipeline._pipe) {
        //     auto& ff = *f.get();
        //     DBGF("%i: Time until future: %i", i++, ff.time_until_future().raw<int>());
        // }
        // DBGF("futures in pipeline :%u, expired futures in pipeline :%u)", _pipeline.contains_futures(),
        // _pipeline.contains_expired_futures());
        // DBGF("freemem: %i", HAL::free_memory());

        while (_pipeline.contains_expired_futures()) {
            // we have futures ready to be processed
            // DBGF("picking up future (has futures in pipeline :%u, has expired futures in pipeline :%u)",
            //      _pipeline.contains_futures(), _pipeline.contains_expired_futures());
            auto future = _pipeline.expire();

            assert(future != nullptr);

            auto event = *reinterpret_cast<Event*>(future.get());
            // DBGF("triggering event with id: %i", ENUM_IDX(event.id()));
            trigger(event);
        }

        // DBG("EventSystem: loop()");
        if (_cfg.delay_between_ticks) {
            if (_pipeline.contains_futures())
                HAL::delay_ms(std::min(_pipeline.timeUntilNextFuture().raw(), _cfg.max_delay_between_ticks.raw()));
            else
                HAL::delay(_cfg.min_delay_between_ticks);
        }
    }

private:
    const Config _cfg;
    Array<EventHandlerMap> _map;

protected:
    Pipeline _pipeline; // caches all events queued for firing
    Pool<Event> _store; // stores all events in memory
};

} // namespace spn::core

// todo: metrics, have a meta event fire and check the skew of it's supposed arrival time and it's actual arrival time