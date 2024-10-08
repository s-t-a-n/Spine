#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/exception.hpp"
#include "spine/filter/filter.hpp"
#include "spine/structure/vector.hpp"

#include <vector>

namespace spn::filter {

template<typename ValueType = double>
/// Stack of filters. Any value provided to this stack will pass through all filters in the stack
class Stack {
public:
    Stack(uint8_t number_of_filters) : _filters() { _filters.reserve(number_of_filters); }

    /// Attach a filter to the provided slot's index
    void attach_filter(std::unique_ptr<Filter<ValueType>> filter) {
        assert(_filters.size() < _filters.max_size());
        _filters.emplace_back(std::move(filter));
    }

    /// Detach a filter for a provided slot's index
    std::unique_ptr<Filter<ValueType>> detach_filter(uint8_t index) {
        assert(index < _filters.size());
        return _filters.remove(index);
    }

    /// Take a new sample, returns filtered value
    ValueType value(const double new_sample) {
        ValueType new_value = new_sample;
        for (auto& sf : _filters) {
            assert(sf.get() != nullptr);
            new_value = sf->value(new_value);
        }
        return new_value;
    }

    /// Returns filtered value
    ValueType value() const { return !_filters.empty() ? _filters.back()->value() : 0; }

    /// Takes a new sample and inserts it into the filterstack
    void new_sample(ValueType sample) { value(sample); }

    /// Reset every filter in the stack to the provided value
    void reset_to(ValueType value) {
        for (auto& sf : _filters) {
            sf->reset_to(value);
        }
    }

    /// Returns pre-allocated slots for filters
    size_t filter_slots() const { return _filters.max_size(); }

private:
    /// todo: cannot use spn/structure/vector; : inplace allocator is missing, or better even to use a linked list
    std::vector<std::unique_ptr<Filter<ValueType>>> _filters;
};

} // namespace spn::filter