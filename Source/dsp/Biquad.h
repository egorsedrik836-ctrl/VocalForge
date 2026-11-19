#pragma once
#include "Common.h"

namespace vf
{
struct BiquadCoefs { double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0; };

namespace design
{
    inline BiquadCoefs norm (double b0, double b1, double b2, double a0, double a1, double a2)
    {
        return { b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0 };
    }
    inline double w0Of (double sr, double f)
    {
        f = std::min (std::max (f, 5.0), sr * 0.49);
        return 2.0 * kPi * f / sr;
    }
    inline BiquadCoefs lowpass (double sr, double f, double q)
    {
        const double w = w0Of (sr, f), c = std::cos (w), al = std::sin (w) / (2.0 * q);
        return norm ((1 - c) / 2, 1 - c, (1 - c) / 2, 1 + al, -2 * c, 1 - al);
    }
    inline BiquadCoefs highpass (double sr, double f, double q)
    {
        const double w = w0Of (sr, f), c = std::cos (w), al = std::sin (w) / (2.0 * q);
        return norm ((1 + c) / 2, -(1 + c), (1 + c) / 2, 1 + al, -2 * c, 1 - al);
    }
    // constant 0 dB peak gain band-pass
    inline BiquadCoefs bandpass (double sr, double f, double q)
    {
        const double w = w0Of (sr, f), c = std::cos (w), al = std::sin (w) / (2.0 * q);
        return norm (al, 0, -al, 1 + al, -2 * c, 1 - al);
    }
    inline BiquadCoefs peaking (double sr, double f, double gainDb, double q)
    {
        const double A = std::pow (10.0, gainDb / 40.0);
        const double w = w0Of (sr, f), c = std::cos (w), al = std::sin (w) / (2.0 * q);
        return norm (1 + al * A, -2 * c, 1 - al * A, 1 + al / A, -2 * c, 1 - al / A);
    }
    inline BiquadCoefs lowShelf (double sr, double f, double gainDb)
    {
        const double A = std::pow (10.0, gainDb / 40.0), sA = std::sqrt (A);
        const double w = w0Of (sr, f), c = std::cos (w), al = std::sin (w) / std::sqrt (2.0);
        return norm (A * ((A + 1) - (A - 1) * c + 2 * sA * al),
                     2 * A * ((A - 1) - (A + 1) * c),
                     A * ((A + 1) - (A - 1) * c - 2 * sA * al),
                     (A + 1) + (A - 1) * c + 2 * sA * al,
                     -2 * ((A - 1) + (A + 1) * c),
                     (A + 1) + (A - 1) * c - 2 * sA * al);
    }
    inline BiquadCoefs highShelf (double sr, double f, double gainDb)
    {
        const double A = std::pow (10.0, gainDb / 40.0), sA = std::sqrt (A);
        const double w = w0Of (sr, f), c = std::cos (w), al = std::sin (w) / std::sqrt (2.0);
        return norm (A * ((A + 1) + (A - 1) * c + 2 * sA * al),
                     -2 * A * ((A - 1) + (A + 1) * c),
                     A * ((A + 1) + (A - 1) * c - 2 * sA * al),
                     (A + 1) - (A - 1) * c + 2 * sA * al,
                     2 * ((A - 1) - (A + 1) * c),
                     (A + 1) - (A - 1) * c - 2 * sA * al);
    }
} // namespace design

struct Biquad
{
    BiquadCoefs c;
    double z1 = 0, z2 = 0;
    void reset() { z1 = z2 = 0; }
    float process (float x)
    {
        const double xx = x;
        const double y = c.b0 * xx + z1;
        z1 = c.b1 * xx - c.a1 * y + z2;
        z2 = c.b2 * xx - c.a2 * y;
        return (float) y;
    }
};
} // namespace vf
