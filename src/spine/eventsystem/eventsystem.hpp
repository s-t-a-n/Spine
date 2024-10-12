#pragma once

#include "spine/core/debugging.hpp"
#include "spine/eventsystem/pipeline.hpp"
#include "spine/structure/array.hpp"
#include "spine/structure/pool.hpp"
#include "spine/structure/time/timers.hpp"
#include "spine/structure/units/si.hpp"
#include "spine/structure/vector.hpp"

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
class EventStore;

/// An event
class Event : public Future {
public:
    using Id = size_t;

public:
    /// A crude data object meant solely to convey a single value known by the sender and receiver.
    class Data {
        // todo: can this be made more useful and less closed for expansion?
    public:
        Data() = default;
        explicit Data(time_ms time) : _value(time) {}
        explicit Data(time_s time) : _value(time) {}
        explicit Data(double value) : _value(value) {}
        explicit Data(uint32_t value) : _value(value) {}

        double value() const;
        uint32_t unsigned_value() const;
        time_ms ms() const;
        time_s s() const;

        bool has_value() const { return bool(_value); }

    private:
        using Value = std::variant<double, uint32_t, time_s, time_ms>;
        std::optional<Value> _value;
    };

public:
    Id id() const { return _id; }
    const Data& data() const { return _data; }

    Event() = default;

private:
    Id _id = {};
    Data _data = {};

    friend EventStore;
    friend EventSystem;
};

class EventHandler {
public:
    EventHandler(EventSystem* evsys) : _evsys(evsys) {}

    virtual void handle_event(const Event& event) { spn_assert(!"Virtual base function called"); };
    virtual ~EventHandler() = default;
    void attach_event_system(EventSystem* evsys) {
        spn_assert(_evsys == nullptr);
        spn_assert(evsys != nullptr);
        _evsys = evsys;
    }

protected:
    EventSystem* evsys() {
        spn_assert(_evsys);
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

    EventSystem(const Config& cfg);

    ~EventSystem();

public:
    std::string pipeline_as_string() const { return _pipeline.to_string(); }

    template<typename T>
    void attach(T id, EventHandler* handler) {
        auto idx = static_cast<Event::Id>(id);
        spn_assert(handler != nullptr);
        spn_assert(idx < _map.size());
        spn_assert(_map[idx]->size() < _cfg.handler_cap);
        _map[idx]->push_back(handler);
    };

    void detach(EventHandler& handler) { spn_assert(!"not implemented"); };

    /// Directly trigger a provided event
    void trigger(const std::shared_ptr<Event>& event);

    /// Directly trigger a provided event
    void trigger(const Event& event);
    ;

    template<typename IdType>
    /// Directly trigger an event for a provided ID
    void trigger(IdType id, const Event::Data& data = {}) {
        trigger(event(id, time_ms(0), data));
    }

    template<typename IdType>
    /// Returns an event for the given id, time_from_now and data
    std::shared_ptr<Event> event(IdType id, const time_ms& time_from_now, const Event::Data& data = {}) {
        auto event = _store.acquire();
        spn_assert(event); // catch gracefully here for use in nested calls
        if (!event) return nullptr;

        event->_id = static_cast<Event::Id>(id);
        event->_data = data;
        event->_time_from_now = time_from_now;
        return event;
    }

    template<typename IdType>
    /// Schedule an event to happen in `time_from_now` time
    void schedule(IdType id, const time_ms& time_from_now, const Event::Data& data = {}) {
        schedule(event(id, time_from_now, data));
    }

    /// Schedule a provided event
    void schedule(std::shared_ptr<Event>&& event) { _pipeline.push(std::move(event)); }

    /// Main loop of event system. It is crucial that this loop is called often enough to fire events in time
    void loop();

private:
    const Config _cfg;
    Array<EventHandlerMap> _map;

protected:
    Pipeline _pipeline; // caches all events queued for firing
    Pool<Event> _store; // stores all events in memory
};

} // namespace spn::core
