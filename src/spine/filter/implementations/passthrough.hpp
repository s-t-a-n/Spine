#pragma once

#include "spine/core/debugging.hpp"
#include "spine/filter/filter.hpp"

#include <cstdint>
#include <utility>

namespace spn::filter {

// Does as it says, nothing. Used in places where for consistency a filter is expected.
// We rely on the Almighty Compiler to squeeze out IOPS
template<typename ValueType = double>
class Passthrough : public Filter<ValueType> {
public:
    struct Config {};

    // K: length of frame
    Passthrough(const Config&& cfg) : _cfg(std::move(cfg)) {
        static_assert(std::is_floating_point_v<ValueType>, "ValueType must be a floating point type");
    }
    ~Passthrough() override = default;

    ValueType value(const ValueType sample) override {
        new_sample(sample);
        return _value;
    }
    void new_sample(const ValueType sample) override { _value = sample; }

    ValueType value() const override { return _value; }

    void reset_to(ValueType value) override { _value = value; }

    ValueType operator()(ValueType sample) { return value(sample); }

private:
    const Config _cfg;
    ValueType _value = 0;
};

} // namespace spn::filter
