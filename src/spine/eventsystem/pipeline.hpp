#pragma once

#include "spine/core/si_units.hpp"
#include "spine/core/timers.hpp"
#include "spine/platform/hal.hpp"

#include <array>
// #include <charconv>
#include <iterator>
#include <numeric>

namespace spn::eventsystem {

using namespace spn::core;

// forward declaration
class Pipeline;

class Future {
public:
    // protected:
    Future(time_ms time_from_now = time_ms{0})
        : _time_from_now(time_from_now), _timer(time::AlarmTimer(time_from_now)){};

    // private:
    bool operator<(const Future& other) const { return _timer.time_from_now() < other._timer.time_from_now(); }
    bool operator==(const Future& other) const { return _timer.time_from_now() == other._timer.time_from_now(); }

    void reschedule(time_ms time_from_now = time_ms{0}) {
        _time_from_now = time_from_now != time_ms(0) ? time_from_now : _time_from_now;
        assert(_time_from_now > time_ms{});
        _timer = time::AlarmTimer(_time_from_now);
    };

    bool expired() { return _timer.expired(); }
    time_ms future() { return _timer.future(); }
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

public:
    Pipeline(size_t events_cap = 128) : _pipe(Pipe(events_cap)) {}

    void push(std::shared_ptr<Future>&& future) {
        assert(_pipe.size() < _pipe.max_size());
        future->reschedule();

        // size_t j = 0;
        // for (auto& spf : _pipe) {
        //     auto& ff = *spf.get();
        //
        //     DBGF("OTHER (me:%p) idx: %u: @ %i ms(in %i ms) ", this, j++, ff.future().raw<int>(),
        //          ff.time_until_future().raw<int>());
        // }

        // todo: move this into sorted container
        size_t i = 0;
        for (auto& other_future : _pipe) {
            // auto& ff = *spf.get();

            // DBGF("OTHER idx: %u: @ %i ms(in %i s) ", i, time_ms(ff.future()).raw<int>(),
            // time_s(ff.time_until_future()).raw<int>());
            if (future->time_until_future().raw<int32_t>() < other_future->time_until_future().raw<int32_t>()) {
                // if (future->time_until_future() < other_future->time_until_future()) {
                // DBGF("future raw: %ld, other raw: %ld", future->time_until_future().raw<int32_t>(),
                // other_future->time_until_future().raw<int32_t>());
                // DBGF("future raw: %i, other raw: %i", future->time_until_future().raw<int>(),
                //      other_future->time_until_future().raw<int>());
                // assert(future->time_until_future().raw<TimeRaw>() <
                // other_future->time_until_future().raw<TimeRaw>()); assert(future->time_until_future().raw<int>() <
                // other_future->time_until_future().raw<int>());
                // DBGF("JUMPING QUEUE idx: %u: @ %i ms(in %i ms) jumps in front of @ %i ms(in %i ms) ", i,
                // future->future().raw<int>(), future->time_until_future().raw<int>(),
                // other_future->future().raw<int>(), other_future->time_until_future().raw<int>());
                auto last_size = _pipe.size();
                // DBGF("Inserting into spot: %i", i);
                _pipe.insert(i, std::move(future));
                assert(last_size + 1 == _pipe.size());

                // i = 0;
                // for (auto& spf : _pipe) {
                //     auto& ff = *spf.get();
                //
                //     DBGF("OTHER POST(me:%p) idx: %u: @ %i ms(in %i s) ", this, j++, time_ms(ff.future()).raw<int>(),
                //          time_s(ff.time_until_future()).raw<int>());
                // }
                return;
            }
            i++;
        }

        // DBGF("pushing back");
        _pipe.push_back(future);
    }

    [[nodiscard]] std::shared_ptr<Future> expire() {
        assert(!_pipe.empty());
        if (_pipe.empty()) return nullptr;

        // size_t j = 0;
        // for (auto& spf : _pipe) {
        //     auto& ff = *spf.get();
        //
        //     DBGF("EXPIRE OTHER (me:%p) idx: %u: @ %i ms(in %i s) ", this, j++, time_ms(ff.future()).raw<int>(),
        //          time_s(ff.time_until_future()).raw<int>());
        // }

        const auto future = _pipe.pop_front();
        // const auto use_count = future.use_count();
        // const auto futere_raw = *future;
        assert(future); // gracefully catch
        assert(future->expired()); // gracefully catch
        if (!future->expired()) return nullptr;
        return future;
    }
    [[nodiscard]] bool contains_expired_futures() const { return contains_futures() && _pipe.peek_front()->expired(); }
    [[nodiscard]] bool contains_futures() const { return !_pipe.empty(); }

    [[nodiscard]] time_ms timeUntilNextFuture() const {
        if (!contains_futures()) {
            return time_ms(0);
        }
        time_ms future = _pipe.peek_front()->future();
        time_ms current = HAL::millis();
        return future <= current ? time_ms(0) : future - current;
    }

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

            // for (auto it = _pipe.begin(); it != _pipe.end(); ++it) {
            //     auto& future = *it;
            //     const auto t_left = future->time_until_future();
            //
            //     // todo: make generic function on time
            //     std::array<char, 32> buffer;
            //     std::string_view unit;
            //     if (time_ms(t_left).raw() < 1000) {
            //         unit = "ms"; // milliseconds
            //         auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(),
            //         time_ms(t_left).raw()); put(std::string_view(buffer.data(), ptr - buffer.data()));
            //     } else if (time_s(t_left).raw() < 900) {
            //         unit = "s"; // seconds
            //         auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(),
            //         time_s(t_left).raw()); put(std::string_view(buffer.data(), ptr - buffer.data()));
            //     } else if (time_m(t_left).raw() > 0) {
            //         unit = "m"; // minute
            //         auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(),
            //         time_m(t_left).raw()); put(std::string_view(buffer.data(), ptr - buffer.data()));
            //     } else {
            //         unit = "h"; // hours
            //         auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(),
            //         time_h(t_left).raw()); put(std::string_view(buffer.data(), ptr - buffer.data()));
            //     }
            //     put(unit);
            //
            //     if (it != _pipe.end()) // Add comma if not the last element
            //         put(", ");
            // }
            // put("]");
            for (auto it = _pipe.begin(); it != _pipe.end(); ++it) {
                auto& future = *it;
                const auto t_left = future->time_until_future();

                std::array<char, 32> buffer;
                std::string_view unit;

                if (time_ms(t_left).raw() < 1000) {
                    unit = "ms";
                    std::snprintf(buffer.data(), buffer.size(), "%ld", time_ms(t_left).raw());
                    put(std::string_view(buffer.data()));
                } else if (time_s(t_left).raw() < 900) {
                    unit = "s";
                    std::snprintf(buffer.data(), buffer.size(), "%ld", time_s(t_left).raw());
                    put(std::string_view(buffer.data()));
                } else if (time_m(t_left).raw() > 0) {
                    unit = "m";
                    std::snprintf(buffer.data(), buffer.size(), "%ld", time_m(t_left).raw());
                    put(std::string_view(buffer.data()));
                } else {
                    unit = "h";
                    std::snprintf(buffer.data(), buffer.size(), "%ld", time_h(t_left).raw());
                    put(std::string_view(buffer.data()));
                }

                put(unit);
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

        if (result.capacity() > total_length)
            DBG("to_string reallocated: was %u, is %u", total_length, result.capacity());
        assert(result.size() <= total_length); // no reallocation
        assert(result.capacity() == total_length); // no reallocation
        return result;
    }

    // private:
    Pipe _pipe;
};

} // namespace spn::eventsystem
