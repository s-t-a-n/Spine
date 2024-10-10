#pragma once

#include "spine/core/debugging.hpp"
#include "spine/platform/gpio.hpp"
#include "spine/structure/array.hpp"
#include "spine/structure/units/si.hpp"

#include <cstdint>
#include <initializer_list>
#include <type_traits>

namespace spn::core::time {

/// A 24hr schedule containing set values for set times
class Schedule {
public:
    struct Block {
        time_s start; // start time of block
        time_s duration; // length of block
        double value; // active value for block
    };

    struct Config {
        const std::initializer_list<Block> blocks;
    };

    Schedule(const Config&& cfg) : Schedule(cfg.blocks) {}

    /// Create a schedule from a list of blocks (note: total time should not exceed 24hrs)
    Schedule(const std::initializer_list<Block>& blocks) : _blocks(blocks) {
        time_s total_time = {};
        for (auto& b : _blocks) {
            spn_assert(time_s(b.duration) > time_s(0));
            total_time += b.duration;
        }
        spn_assert(total_time <= time_h(24));
    }

    /// Returns the value in the block of `t` or 0
    double value_at(const time_s t) const {
        for (auto& b : _blocks) {
            const auto is_today = t >= b.start && t < b.start + b.duration;
            const auto is_after_midnight = b.start + b.duration > time_h(24) && t < b.start + b.duration - time_h(24);
            if (is_today || is_after_midnight) return b.value;
        }
        return {};
    }

    /// Returns the start of the block after time `t` or the maximum length of 24 hours
    time_s start_of_next_block(const time_s t) const {
        for (auto& b : _blocks) {
            if (t < b.start) return b.start;
        }
        return time_h(24);
    }

private:
    spn::structure::Array<Block> _blocks;
};

} // namespace spn::core::time
