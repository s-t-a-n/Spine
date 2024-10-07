#pragma once

#include "spine/core/utils/string.hpp"
#include "spine/core/utils/time_repr.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/time/timers.hpp"
#include "spine/structure/units/si.hpp"

// #include <charconv>
#include <iterator>

namespace spn::eventsystem {

using namespace spn::core;

// forward declaration
class Pipeline;

/// A moment in the future determined with time_from_now (relative to the system clock)
class Future {
public:
    Future(time_ms time_from_now = time_ms{0})
        : _time_from_now(time_from_now), _timer(time::AlarmTimer(time_from_now)) {};

    bool operator<(const Future& other) const { return _timer.time_from_now() < other._timer.time_from_now(); }
    bool operator==(const Future& other) const { return _timer.time_from_now() == other._timer.time_from_now(); }

    /// Reschedule a future to happen at a different moment (this has no effect when the future is already in the
    /// pipeline)
    void reschedule(time_ms time_from_now = time_ms{0}) {
        _time_from_now = time_from_now != time_ms(0) ? time_from_now : _time_from_now;
        assert(_time_from_now > time_ms{});
        _timer = time::AlarmTimer(_time_from_now);
    };

    /// Returns true if the event is ready to fire
    bool expired() { return _timer.expired(); }

    /// Returns the moment (in absolute system time) in the future when the event should fire
    time_ms future() { return _timer.future(); }

    /// Returns the time (relative from the system time) until the event should fire
    time_ms time_until_future() { return _timer.time_from_now(); }

protected:
    time_ms _time_from_now;

private:
    time::AlarmTimer _timer;

    friend class Pipeline;
};

class Pipeline {
public:
    using Pipe = spn::structure::Vector<std::shared_ptr<Future>>;

    Pipeline(size_t events_cap = 128) : _pipe(Pipe(events_cap)) {}

    /// Push a new future in the pipeline (inserting it in chronological order)
    void push(std::shared_ptr<Future>&& future) {
        assert(_pipe.size() < _pipe.max_size());
        future->reschedule();

        size_t i = 0;
        for (auto& other_future : _pipe) {
            if (future->time_until_future().raw<int32_t>() < other_future->time_until_future().raw<int32_t>()) {
                [[maybe_unused]] auto last_size = _pipe.size();
                _pipe.insert(i, std::move(future));
                assert(last_size + 1 == _pipe.size());
                return;
            }
            i++;
        }
        _pipe.push_back(future);
    }

    /// Takes the first next future to fire from the pipeline
    [[nodiscard]] std::shared_ptr<Future> expire() {
        assert(!_pipe.empty());
        if (_pipe.empty()) return nullptr;

        const auto future = _pipe.pop_front();
        assert(future); // gracefully catch
        assert(future->expired()); // gracefully catch
        if (!future->expired()) return nullptr;
        return future;
    }

    /// Returns true if the pipeline contains any futures that are ready to fire
    [[nodiscard]] bool contains_expired_futures() const { return contains_futures() && _pipe.peek_front()->expired(); }

    /// Returns true if the pipeline contains any futures whatsoever
    [[nodiscard]] bool contains_futures() const { return !_pipe.empty(); }

    /// Returns the time (relative to the system clock) until the first next expirable future
    [[nodiscard]] time_ms time_until_next_future() const {
        if (!contains_futures()) {
            return time_ms(0);
        }
        time_ms future = _pipe.peek_front()->future();
        time_ms current = HAL::millis();
        return future <= current ? time_ms(0) : future - current;
    }

    /// Returns a string representation of the Pipeline's content
    std::string to_string() const {
        auto pipeline_repr = [&](std::string* s, bool length_only) -> size_t {
            size_t len = 0;
            auto put = [&](std::string_view i) {
                if (!length_only) {
                    *s += i;
                }
                len += i.size();
            };
            put("Pipeline: [");
            for (auto it = _pipe.begin(); it != _pipe.end(); ++it) {
                auto& future = *it;
                put(utils::repr(future->time_until_future()));
                if (it != _pipe.end()) put(", ");
            }
            put("]");
            return len;
        };

        // First pass to calculate total length
        size_t total_length = pipeline_repr(nullptr, true);

        // Reserve capacity to avoid multiple allocations
        std::string result;
        result.reserve(total_length);

        // Second pass to build the string
        pipeline_repr(&result, false);

        assert(result.size() <= total_length); // no reallocation
        assert(result.capacity() == total_length); // no reallocation
        return result;
    }

    /// Returns the internal pipe for external inspection (probably only under testing)
    const Pipe& pipe() const { return _pipe; }

private:
    Pipe _pipe;
};

} // namespace spn::eventsystem
