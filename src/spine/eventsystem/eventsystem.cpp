#include "spine/eventsystem/eventsystem.hpp"

namespace spn::core {

float Event::Data::value() const {
    spn_assert(_value);
    if (!_value) return {};
    spn_assert(std::holds_alternative<float>(*_value));
    return std::get<float>(*_value);
}

uint32_t Event::Data::unsigned_value() const {
    spn_assert(_value);
    if (!_value) return {};
    spn_assert(std::holds_alternative<uint32_t>(*_value));
    return std::get<uint32_t>(*_value);
}

k_time_ms Event::Data::ms() const {
    spn_assert(_value);
    if (!_value) return {};
    spn_assert(std::holds_alternative<k_time_ms>(*_value));
    return std::get<k_time_ms>(*_value);
}

k_time_s Event::Data::s() const {
    spn_assert(_value);
    if (!_value) return {};
    spn_assert(std::holds_alternative<k_time_s>(*_value));
    return std::get<k_time_s>(*_value);
}

EventSystem::EventSystem(const EventSystem::Config& cfg) //
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

EventSystem::~EventSystem() {
    for (size_t i = 0; i < _map.size(); i++) {
        _map[i].reset();
    }
    for (size_t i = 0; i < _store.size(); ++i) {
        auto ev = _store.depopulate();
        ev.reset();
    }
    spn_assert(_store.size() == 0);
}

void EventSystem::trigger(const std::shared_ptr<Event>& event) {
    spn_assert(event);
    trigger(*event.get());
}

void EventSystem::trigger(const Event& event) {
    const auto id = event.id();
    spn_assert(id < _map.size());
    spn_assert(_map[id] != nullptr);
    spn_assert(!_map[id]->empty());
    for (auto handler : *_map[id]) {
        handler->handle_event(event);
    }
}

void EventSystem::loop() {
    while (_pipeline.contains_expired_futures()) {
        // we have futures ready to be processed
        auto future = _pipeline.expire();
        spn_assert(future != nullptr);
        auto event = *reinterpret_cast<Event*>(future.get());
        trigger(event);
    }

    if (_cfg.delay_between_ticks) {
        if (_pipeline.contains_futures())
            HAL::delay_ms(std::min(_pipeline.time_until_next_future().raw(), _cfg.max_delay_between_ticks.raw()));
        else
            HAL::delay(_cfg.min_delay_between_ticks);
    }
}

} // namespace spn::core
