#include "spine/eventsystem/pipeline.hpp"

#include "spine/core/utils/time_repr.hpp"

namespace spn::eventsystem {

Future::Future(time_ms time_from_now) : _time_from_now(time_from_now), _timer(AlarmTimer(time_from_now)) {}

void Future::reschedule(time_ms time_from_now) {
    _time_from_now = time_from_now != time_ms(0) ? time_from_now : _time_from_now;
    spn_assert(_time_from_now > time_ms{});
    _timer = AlarmTimer(_time_from_now);
}

std::string Pipeline::to_string() const {
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

    spn_assert(result.size() <= total_length); // no reallocation
    spn_assert(result.capacity() == total_length); // no reallocation
    return result;
}

time_ms Pipeline::time_until_next_future() const {
    if (!contains_futures()) {
        return time_ms(0);
    }
    time_ms future = _pipe.peek_front()->future();
    time_ms current = HAL::millis();
    return future <= current ? time_ms(0) : future - current;
}

std::shared_ptr<Future> Pipeline::expire() {
    spn_assert(!_pipe.empty());
    if (_pipe.empty()) return nullptr;

    const auto future = _pipe.pop_front();
    spn_assert(future); // gracefully catch
    spn_assert(future->expired()); // gracefully catch
    if (!future->expired()) return nullptr;
    return future;
}

void Pipeline::push(std::shared_ptr<Future>&& future) {
    spn_assert(_pipe.size() < _pipe.max_size());
    future->reschedule();

    size_t i = 0;
    for (auto& other_future : _pipe) {
        if (future->time_until_future().raw<int32_t>() < other_future->time_until_future().raw<int32_t>()) {
            [[maybe_unused]] auto last_size = _pipe.size();
            _pipe.insert(i, std::move(future));
            spn_assert(last_size + 1 == _pipe.size());
            return;
        }
        i++;
    }
    _pipe.push_back(future);
}

} // namespace spn::eventsystem