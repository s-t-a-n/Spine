#pragma once

#include "spine/core/debugging.hpp"
#include "spine/platform/gpio.hpp"
#include "spine/structure/array.hpp"
#include "spine/structure/units/si.hpp"

#include <cstdint>
#include <initializer_list>
#include <type_traits>

namespace spn::structure::time {

/// A 24hr schedule containing set values for set times
class Schedule {
public:
    struct Block {
        k_time_s start; // start time of block
        k_time_s duration; // length of block
        double value; // active value for block
    };

    struct Config {
        const std::initializer_list<Block> blocks;
    };

    Schedule(const Config&& cfg);

    /// Create a schedule from a list of blocks (note: total time should not exceed 24hrs)
    Schedule(const std::initializer_list<Block>& blocks);

    /// Returns the value in the block of `t` or 0
    double value_at(const k_time_s t) const;

    /// Returns the start of the block after time `t` or the maximum length of 24 hours
    k_time_s start_of_next_block(const k_time_s t) const;

private:
    spn::structure::Array<Block> _blocks;
};

} // namespace spn::structure::time
