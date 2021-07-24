#pragma once

#include "spine/core/debugging.hpp"
#include "spine/core/exception.hpp"
#include "spine/filter/filter.hpp"
#include "spine/structure/vector.hpp"

#include <vector>

namespace spn::filter {

template<typename ValueType = double>
class Stack {
public:
    // Stack(const std::initializer_list<std::unique_ptr<Filter<ValueType>>> filters) : _filters(filters) {}
    Stack(uint8_t number_of_filters) : _filters() { _filters.reserve(number_of_filters); }

    void attach_filter(std::unique_ptr<Filter<ValueType>> filter) {
        // DBGF("size: %i, max_size: %i", _filters.size(), _filters.max_size());
        assert(_filters.size() < _filters.max_size());
        _filters.emplace_back(std::move(filter));
    }

    Filter<ValueType> detach_filter(uint8_t index) {
        assert(index < _filters.size());
        return std::move(_filters.remove(index));
    }

    double value(const double new_sample) {
        double new_value = new_sample;
        for (auto& sf : _filters) {
            new_value = sf->value(new_value);
        }
        return new_value;
    }

    ValueType value() const { return !_filters.empty() ? _filters.back()->value() : 0; }

    void new_sample(ValueType sample) { value(sample); }

    void reset_to(ValueType value) {
        for (auto& sf : _filters) {
            sf->reset_to(value);
        }
    }

    size_t max_number_of_filters() const { return _filters.max_size(); }

private:
    /// cannot use internal Vector; todo: inplace allocator
    /// todo: use linked list instead
    std::vector<std::unique_ptr<Filter<ValueType>>> _filters;
};

} // namespace spn::filter