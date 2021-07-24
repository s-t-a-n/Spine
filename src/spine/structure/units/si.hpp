#pragma once

#include "spine/structure/units/unit.hpp"

#include <cmath>
#include <cstdint>
#include <ctime>

namespace spn::detail {

using namespace spn::core;

// Time, with unit: second
struct TimeU {};
using TimeRaw = time_t;
struct MicroSecondsTag : public Unit<TimeU, MicroSecondsTag, TimeRaw> {
    using Unit<TimeU, MicroSecondsTag, TimeRaw>::Unit;
    static constexpr double Magnitude = 1e-6;
};
struct MilliSecondsTag : public Unit<TimeU, MilliSecondsTag, TimeRaw> {
    using Unit<TimeU, MilliSecondsTag, TimeRaw>::Unit;
    static constexpr double Magnitude = 1e-3;
};
struct SecondsTag : public Unit<TimeU, SecondsTag, TimeRaw> {
    using Unit<TimeU, SecondsTag, TimeRaw>::Unit;
    static constexpr double Magnitude = 1.0;
};
struct MinutesTag : public Unit<TimeU, MinutesTag, TimeRaw> {
    using Unit<TimeU, MinutesTag, TimeRaw>::Unit;
    static constexpr double Magnitude = 60.0;
};
struct HoursTag : public Unit<TimeU, HoursTag, TimeRaw> {
    using Unit<TimeU, HoursTag, TimeRaw>::Unit;
    static constexpr double Magnitude = 3600.0; // 60*60
};
struct DaysTag : public Unit<TimeU, DaysTag, TimeRaw> {
    using Unit<TimeU, DaysTag, TimeRaw>::Unit;
    static constexpr double Magnitude = 86400.0; // 60*60*24
};

// Potential, with unit: voltage
struct PotentialU {};
using PotentialRaw = double;
struct MicroVoltsTag : public Unit<PotentialU, MicroVoltsTag, PotentialRaw> {
    using Unit<PotentialU, MicroVoltsTag, PotentialRaw>::Unit;
    static constexpr double Magnitude = 1e-6;
};
struct MilliVoltsTag : public Unit<PotentialU, MilliVoltsTag, PotentialRaw> {
    using Unit<PotentialU, MilliVoltsTag, PotentialRaw>::Unit;
    static constexpr double Magnitude = 1e-3;
};
struct VoltsTag : public Unit<PotentialU, VoltsTag, PotentialRaw> {
    using Unit<PotentialU, VoltsTag, PotentialRaw>::Unit;
    static constexpr double Magnitude = 1.0;
};

// Current, with unit: ampere
struct CurrentU {};
using CurrentRaw = double;
struct MicroAmpsTag : public Unit<CurrentU, MicroAmpsTag, CurrentRaw> {
    using Unit<CurrentU, MicroAmpsTag, CurrentRaw>::Unit;
    static constexpr double Magnitude = 1e-6;
};
struct MilliAmpsTag : public Unit<CurrentU, MilliAmpsTag, CurrentRaw> {
    using Unit<CurrentU, MilliAmpsTag, CurrentRaw>::Unit;
    static constexpr double Magnitude = 1e-3;
};
struct AmpsTag : public Unit<CurrentU, AmpsTag, CurrentRaw> {
    using Unit<CurrentU, AmpsTag, CurrentRaw>::Unit;
    static constexpr double Magnitude = 1.0;
};

// Volume, with unit: litre
struct VolumeU {};
using VolumeRaw = double;
struct MilliLitersTag : public Unit<VolumeU, MilliLitersTag, VolumeRaw> {
    using Unit<VolumeU, MilliLitersTag, VolumeRaw>::Unit;
    static constexpr double Magnitude = 1e-3;
};
struct LitersTag : public Unit<VolumeU, LitersTag, VolumeRaw> {
    using Unit<VolumeU, LitersTag, VolumeRaw>::Unit;
    static constexpr double Magnitude = 1.0;
};

// Weight, with unit: gram
struct WeightU {};
using WeightRaw = double;
struct MicroGramsTag : public Unit<WeightU, MicroGramsTag, WeightRaw> {
    using Unit<WeightU, MicroGramsTag, WeightRaw>::Unit;
    static constexpr double Magnitude = 1e-6;
};
struct MilliGramsTag : public Unit<WeightU, MilliGramsTag, WeightRaw> {
    using Unit<WeightU, MilliGramsTag, WeightRaw>::Unit;
    static constexpr double Magnitude = 1e-3;
};
struct GramsTag : public Unit<WeightU, GramsTag, WeightRaw> {
    using Unit<WeightU, GramsTag, WeightRaw>::Unit;
    static constexpr double Magnitude = 1.0;
};
struct KiloGramsTag : public Unit<WeightU, KiloGramsTag, WeightRaw> {
    using Unit<WeightU, KiloGramsTag, WeightRaw>::Unit;
    static constexpr double Magnitude = 1e3;
};

// Length, with unit: meter
struct LengthU {};
using LengthRaw = double;
struct NanoMetersTag : public Unit<LengthU, NanoMetersTag, LengthRaw> {
    using Unit<LengthU, NanoMetersTag, LengthRaw>::Unit;
    static constexpr double Magnitude = 1e-9;
};
struct MicroMetersTag : public Unit<LengthU, MicroMetersTag, LengthRaw> {
    using Unit<LengthU, MicroMetersTag, LengthRaw>::Unit;
    static constexpr double Magnitude = 1e-6;
};
struct MilliMetersTag : public Unit<LengthU, MilliMetersTag, LengthRaw> {
    using Unit<LengthU, MilliMetersTag, LengthRaw>::Unit;
    static constexpr double Magnitude = 1e-3;
};
struct CentiMetersTag : public Unit<LengthU, CentiMetersTag, LengthRaw> {
    using Unit<LengthU, CentiMetersTag, LengthRaw>::Unit;
    static constexpr double Magnitude = 1e-2;
};
struct DeciMetersTag : public Unit<LengthU, DeciMetersTag, LengthRaw> {
    using Unit<LengthU, DeciMetersTag, LengthRaw>::Unit;
    static constexpr double Magnitude = 1e-1;
};
struct MetersTag : public Unit<LengthU, MetersTag, LengthRaw> {
    using Unit<LengthU, MetersTag, LengthRaw>::Unit;
    static constexpr double Magnitude = 1.0;
};
struct KiloMetersTag : public Unit<LengthU, KiloMetersTag, LengthRaw> {
    using Unit<LengthU, KiloMetersTag, LengthRaw>::Unit;
    static constexpr double Magnitude = 1e3;
};

// Temperature, with unit: centigrade
struct TemperatureU {};
using TemperatureRaw = double;
struct CentigradesTag : public Unit<TemperatureU, CentigradesTag, TemperatureRaw> {
    using Unit<TemperatureU, CentigradesTag, TemperatureRaw>::Unit;
    static constexpr double Magnitude = 1.0;
};

} // namespace spn::detail

// Time, with unit: second
using time_us = spn::core::Unit<spn::detail::TimeU, spn::detail::MicroSecondsTag, spn::detail::TimeRaw>;
using time_ms = spn::core::Unit<spn::detail::TimeU, spn::detail::MilliSecondsTag, spn::detail::TimeRaw>;
using time_s = spn::core::Unit<spn::detail::TimeU, spn::detail::SecondsTag, spn::detail::TimeRaw>;
using time_m = spn::core::Unit<spn::detail::TimeU, spn::detail::MinutesTag, spn::detail::TimeRaw>;
using time_h = spn::core::Unit<spn::detail::TimeU, spn::detail::HoursTag, spn::detail::TimeRaw>;
using time_d = spn::core::Unit<spn::detail::TimeU, spn::detail::DaysTag, spn::detail::TimeRaw>;

// Potential, with unit: voltage
using volt_u = spn::core::Unit<spn::detail::PotentialU, spn::detail::MicroVoltsTag, spn::detail::PotentialRaw>;
using volt_m = spn::core::Unit<spn::detail::PotentialU, spn::detail::MilliVoltsTag, spn::detail::PotentialRaw>;
using volt = spn::core::Unit<spn::detail::PotentialU, spn::detail::VoltsTag, spn::detail::PotentialRaw>;

// Current, with unit: ampere
using amp_u = spn::core::Unit<spn::detail::CurrentU, spn::detail::MicroAmpsTag, spn::detail::CurrentRaw>;
using amp_m = spn::core::Unit<spn::detail::CurrentU, spn::detail::MilliAmpsTag, spn::detail::CurrentRaw>;
using amp = spn::core::Unit<spn::detail::CurrentU, spn::detail::AmpsTag, spn::detail::CurrentRaw>;

// Volume, with unit: litre
using litre_ml = spn::core::Unit<spn::detail::VolumeU, spn::detail::MilliLitersTag, spn::detail::VolumeRaw>;
using litre = spn::core::Unit<spn::detail::VolumeU, spn::detail::LitersTag, spn::detail::VolumeRaw>;

// Weight, with unit: gram
using gram_u = spn::core::Unit<spn::detail::WeightU, spn::detail::MicroGramsTag, spn::detail::WeightRaw>;
using gram_m = spn::core::Unit<spn::detail::WeightU, spn::detail::MilliGramsTag, spn::detail::WeightRaw>;
using gram = spn::core::Unit<spn::detail::WeightU, spn::detail::GramsTag, spn::detail::WeightRaw>;
using gram_k = spn::core::Unit<spn::detail::WeightU, spn::detail::KiloGramsTag, spn::detail::WeightRaw>;

// Length, with unit: meter
using meter_n = spn::core::Unit<spn::detail::LengthU, spn::detail::NanoMetersTag, spn::detail::LengthRaw>;
using meter_u = spn::core::Unit<spn::detail::LengthU, spn::detail::MicroMetersTag, spn::detail::LengthRaw>;
using meter_m = spn::core::Unit<spn::detail::LengthU, spn::detail::MilliMetersTag, spn::detail::LengthRaw>;
using meter_c = spn::core::Unit<spn::detail::LengthU, spn::detail::CentiMetersTag, spn::detail::LengthRaw>;
using meter_d = spn::core::Unit<spn::detail::LengthU, spn::detail::DeciMetersTag, spn::detail::LengthRaw>;
using meter = spn::core::Unit<spn::detail::LengthU, spn::detail::MetersTag, spn::detail::LengthRaw>;
using meter_k = spn::core::Unit<spn::detail::LengthU, spn::detail::KiloMetersTag, spn::detail::LengthRaw>;

// Temperature, with unit: centigrade
using centigrade = spn::core::Unit<spn::detail::TemperatureU, spn::detail::CentigradesTag, spn::detail::TemperatureRaw>;
