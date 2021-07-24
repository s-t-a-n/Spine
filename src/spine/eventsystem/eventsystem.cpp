// // #if defined(ARDUINO) && not defined(UNITTEST)
// // #    include <Arduino.h>
// // #elif defined(ARDUINO)
// // #    include <ArduinoFake.h>
// // #endif

// #include "spine/eventsystem/eventsystem.hpp"

// #include "spine/core/debugging.hpp"

// #include <AH/Math/MinMaxFix.hpp>

// using namespace spn::eventsystem;
// using namespace spn::core;

// // Event::Event(Id id, const Data& data, const time_ms& time_from_now) : Future(time_from_now), _id(id),
// _data(data){};
// // Event::Event(Id id, const Data& data, const time_s& time_from_now)
// // : Future(time_ms(time_from_now)), _id(id), _data(data){};
// // Event::Id Event::id() { return _id; }
// // Event::Data Event::data() { return _data; }

// EventHandler::EventHandler(EventSystem* evsys) : _evsys(evsys) {}

// void EventSystem::detach(EventHandler& handler){};

// // template<typename IdType>
// // Event* EventSystem::new_event(IdType id, const time_ms& time_from_now, const Event::Data& data) {
// // return _evsysctrl->new_event(static_cast<Event::Id>(id), time_from_now, data);
// // }

// // void EventSystemController::schedule(Event* event) { }

// // String EventSystemController::to_string() const {
// //     auto s = String();

// //     s += "EventSystem: ";
// //     // s += _pipeline.to_string();
// //     return s;
// // }
