#pragma once

namespace spn::structure {

/// a naive tuple implementation
template<typename T0, typename T1>
struct Tuple {
public:
    Tuple(const T0& first, const T1& second) : m_t0(first), m_t1(second) {}

    inline T0& first() const { return const_cast<T0&>(m_t0); };
    inline T1& second() const { return const_cast<T1&>(m_t1); }

private:
    const T0 m_t0;
    const T1 m_t1;
};

} // namespace spn::structure
