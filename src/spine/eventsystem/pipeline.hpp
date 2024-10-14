#pragma once

#include "spine/core/utils/string.hpp"
#include "spine/core/utils/time_repr.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/time/timers.hpp"
#include "spine/structure/units/si.hpp"

namespace spn::eventsystem {

using namespace spn::core;

// forward declaration
class Pipeline;

/// A moment in the future determined with time_from_now (relative to the system clock)
class Future {
public:
    Future(k_time_ms time_from_now = k_time_ms{0});

    bool operator<(const Future& other) const { return _timer.time_from_now() < other._timer.time_from_now(); }
    bool operator==(const Future& other) const { return _timer.time_from_now() == other._timer.time_from_now(); }

    /// Reschedule a future to happen at a different moment (this has no effect when the future is already in the
    /// pipeline)
    void reschedule(k_time_ms time_from_now = k_time_ms{0});

    /// Returns true if the event is ready to fire
    bool expired() { return _timer.expired(); }

    /// Returns the moment (in absolute system time) in the future when the event should fire
    k_time_ms future() { return _timer.future(); }

    /// Returns the time (relative from the system time) until the event should fire
    k_time_ms time_until_future() { return _timer.time_from_now(); }

protected:
    k_time_ms _time_from_now;

private:
    using AlarmTimer = spn::structure::time::AlarmTimer;

    AlarmTimer _timer;

    friend class Pipeline;
};

class Pipeline {
public:
    using Pipe = spn::structure::Vector<std::shared_ptr<Future>>;

    Pipeline(size_t events_cap = 128) : _pipe(Pipe(events_cap)) {}

    /// Push a new future in the pipeline (inserting it in chronological order)
    void push(std::shared_ptr<Future>&& future);

    /// Takes the first next future to fire from the pipeline
    [[nodiscard]] std::shared_ptr<Future> expire();

    /// Returns true if the pipeline contains any futures that are ready to fire
    [[nodiscard]] bool contains_expired_futures() const { return contains_futures() && _pipe.peek_front()->expired(); }

    /// Returns true if the pipeline contains any futures whatsoever
    [[nodiscard]] bool contains_futures() const { return !_pipe.empty(); }

    /// Returns the time (relative to the system clock) until the first next expirable future
    [[nodiscard]] k_time_ms time_until_next_future() const;

    /// Returns a string representation of the Pipeline's content
    std::string to_string() const;

    /// Returns the internal pipe for external inspection (probably only under testing)
    const Pipe& pipe() const { return _pipe; }

private:
    Pipe _pipe;
};

} // namespace spn::eventsystem
