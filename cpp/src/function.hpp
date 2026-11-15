#pragma once

namespace mc {

// --- ВРЕМЕНЕН/ПРИМЕРЕН ИНДИВИДУАЛЕН ВАРИАНТ --------------------------------
// Виж VARIANT.md. При получаване на официалния вариант се сменят само f(x),
// kA, kB и kReference тук, и идентично в java/src/Functions.java.
//
// f(x) = 4 / (1 + x^2) на [0, 1]; интегралът е точно pi.
inline double f(double x) {
    return 4.0 / (1.0 + x * x);
}

constexpr double kA = 0.0;
constexpr double kB = 1.0;
constexpr double kReference = 3.14159265358979323846;
// ----------------------------------------------------------------------------

} // namespace mc
