#pragma once

namespace spn::filter {

template<typename ValueType = float>
/// Interface class Filter (used in filterstack)
class Filter {
public:
    virtual ~Filter() = default;

    /// Take a new sample, returns filtered value
    virtual ValueType value(ValueType new_sample) = 0;

    /// Returns filtered value
    virtual ValueType value() const = 0;

    /// Takes a new sample (syntax sugar)
    virtual void new_sample(ValueType sample) = 0;

    /// Reset the filter's internal value to the provided value
    virtual void reset_to(ValueType value) = 0;

private:
};

} // namespace spn::filter
