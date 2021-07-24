#pragma once

namespace spn::filter {

template<typename ValueType = double>
class Filter {
public:
    virtual ~Filter() = default;

    virtual ValueType value(ValueType new_sample) = 0;
    virtual ValueType value() const = 0;
    virtual void new_sample(ValueType sample) = 0;
    virtual void reset_to(ValueType value) = 0;

private:
};

} // namespace spn::filter