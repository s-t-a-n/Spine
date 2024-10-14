#pragma once

namespace spn::structure::units {

// SI magnitude tags
struct Atto {
    static constexpr double Magnitude = 1e-18;
};
struct Femto {
    static constexpr double Magnitude = 1e-15;
};
struct Pico {
    static constexpr double Magnitude = 1e-12;
};
struct Nano {
    static constexpr double Magnitude = 1e-9;
};
struct Micro {
    static constexpr double Magnitude = 1e-6;
};
struct Milli {
    static constexpr double Magnitude = 1e-3;
};
struct Centi {
    static constexpr double Magnitude = 1e-2;
};
struct Deci {
    static constexpr double Magnitude = 1e-1;
};
struct Deca {
    static constexpr double Magnitude = 1e1;
};
struct Hecto {
    static constexpr double Magnitude = 1e2;
};
struct Kilo {
    static constexpr double Magnitude = 1e3;
};
struct Mega {
    static constexpr double Magnitude = 1e6;
};
struct Giga {
    static constexpr double Magnitude = 1e9;
};
struct Tera {
    static constexpr double Magnitude = 1e12;
};
struct Peta {
    static constexpr double Magnitude = 1e15;
};
struct Exa {
    static constexpr double Magnitude = 1e18;
};

struct Base {
    static constexpr double Magnitude = 1.0;
};

// time types
struct Minute {
    static constexpr double Magnitude = 60.0;
};
struct Hour {
    static constexpr double Magnitude = 60 * 60;
};
struct Day {
    static constexpr double Magnitude = 60 * 60 * 24;
};
struct Week {
    static constexpr double Magnitude = Day::Magnitude * 7;
};
struct Month {
    static constexpr double Magnitude = Week::Magnitude * 4;
};
struct Year {
    static constexpr double Magnitude = Day::Magnitude * 365;
};

} // namespace spn::structure::units