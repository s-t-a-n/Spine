#include "spine/structure/time/schedule.hpp"

namespace spn::structure::time {

Schedule::Schedule(const Schedule::Config&& cfg) : Schedule(cfg.blocks) {}

Schedule::Schedule(const std::initializer_list<Block>& blocks) : _blocks(blocks) {
    time_s total_time = {};
    for (auto& b : _blocks) {
        spn_assert(time_s(b.duration) > time_s(0));
        total_time += b.duration;
    }
    spn_assert(total_time <= time_h(24));
}

double Schedule::value_at(const time_s t) const {
    for (auto& b : _blocks) {
        const auto is_today = t >= b.start && t < b.start + b.duration;
        const auto is_after_midnight = b.start + b.duration > time_h(24) && t < b.start + b.duration - time_h(24);
        if (is_today || is_after_midnight) return b.value;
    }
    return {};
}

time_s Schedule::start_of_next_block(const time_s t) const {
    for (auto& b : _blocks) {
        if (t < b.start) return b.start;
    }
    return time_h(24);
}

} // namespace spn::structure::time