#pragma once

namespace spn::structure::units {

// SI magnitude tags
struct Atto {
    static constexpr float Magnitude = 1e-18f;
};
struct Femto {
    static constexpr float Magnitude = 1e-15f;
};
struct Pico {
    static constexpr float Magnitude = 1e-12f;
};
struct Nano {
    static constexpr float Magnitude = 1e-9f;
};
struct Micro {
    static constexpr float Magnitude = 1e-6f;
};
struct Milli {
    static constexpr float Magnitude = 1e-3f;
};
struct Centi {
    static constexpr float Magnitude = 1e-2f;
};
struct Deci {
    static constexpr float Magnitude = 1e-1f;
};
struct Deca {
    static constexpr float Magnitude = 1e1f;
};
struct Hecto {
    static constexpr float Magnitude = 1e2f;
};
struct Kilo {
    static constexpr float Magnitude = 1e3f;
};
struct Mega {
    static constexpr float Magnitude = 1e6f;
};
struct Giga {
    static constexpr float Magnitude = 1e9f;
};
struct Tera {
    static constexpr float Magnitude = 1e12f;
};
struct Peta {
    static constexpr float Magnitude = 1e15f;
};
struct Exa {
    static constexpr float Magnitude = 1e18f;
};

struct Base {
    static constexpr float Magnitude = 1.0f;
};

// time types
struct Minute {
    static constexpr float Magnitude = 60.0f;
};
struct Hour {
    static constexpr float Magnitude = 60.0f * 60.0f;
};
struct Day {
    static constexpr float Magnitude = 60.0f * 60.0f * 24.0f;
};
struct Week {
    static constexpr float Magnitude = Day::Magnitude * 7.0f;
};
struct Month {
    static constexpr float Magnitude = Week::Magnitude * 4.0f;
};
struct Year {
    static constexpr float Magnitude = Day::Magnitude * 365.0f;
};

} // namespace spn::structure::units
