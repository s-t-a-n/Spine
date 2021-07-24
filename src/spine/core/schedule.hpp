#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/time.hpp"
#include "spine/platform/gpio.hpp"
#include "spine/structure/array.hpp"

#include <cstdint>
#include <initializer_list>
#include <type_traits>

namespace spn::core::time {

using spn::structure::Array;

class Schedule {
public:
    struct Block {
        time_s start;
        time_s duration;
        double value;
    };

    struct Config {
        const std::initializer_list<Block> blocks;
    };

    Schedule(const Config&& cfg) : Schedule(cfg.blocks) {}

    Schedule(const std::initializer_list<Block>& blocks) : _blocks(blocks) {
        time_s total_time = {};
        // DBGF("Schedule constructor: ");
        for (auto& b : _blocks) {
            assert(time_s(b.duration) > time_s(0));
            total_time += b.duration;
            // DBGF("Schedule constructor: start %i, duration %i, value %f", b.start.raw<>(), b.duration.raw<>(),
            // b.value);
        }
        assert(total_time <= time_h(24));
    }

    /// returns the value in the block of `t` or 0
    double value_at(const time_s t) const {
        for (auto& b : _blocks) {
            // DBGF("Schedule value_at: start %i, duration %i, value %f", b.start.raw<>(), b.duration.raw<>(), b.value);
            const auto is_today = t >= b.start && t < b.start + b.duration;
            const auto is_after_midnight = b.start + b.duration > time_h(24) && t < b.start + b.duration - time_h(24);
            if (is_today || is_after_midnight) return b.value;
        }
        return {};
    }

    /// returns the start of the block after time `t` or the maximum length of 24 hours
    time_s start_of_next_block(const time_s t) const {
        for (auto& b : _blocks) {
            // DBGF("Schedule start_of_next_block: start %i, duration %i, value %f", b.start.raw<>(),
            // b.duration.raw<>(), b.value);

            if (t < b.start) {
                // DBGF("start : %i s, t: %i s, start - t: %i h", b.start.raw<>(), t.raw<>());
                return b.start;
            }
        }
        // DBGF("time_h 24 : %i, t: %i, 24 - t: %i", time_ms(time_h(24)).raw<>(), time_ms(t).raw<>(),
        // time_ms(time_h(24)).raw<>());
        return time_h(24);
    }

private:
    Array<Block> _blocks;
};

} // namespace spn::core::time