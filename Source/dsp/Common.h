#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

namespace vf
{
constexpr double kPi = 3.14159265358979323846;

inline float dbToLin (float db) { return std::exp (db * 0.11512925465f); }
inline float linToDb (float lin) { return 8.685889638f * std::log (std::max (lin, 1.0e-9f)); }
inline float clampf (float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

// one-pole exponential coefficient for a time constant in milliseconds
inline float msToCoef (float ms, double sr)
{
    if (ms <= 0.0f) return 0.0f;
    return (float) std::exp (-1.0 / (0.001 * (double) ms * sr));
}

struct Smoother
{
    float cur = 0.0f, target = 0.0f, coef = 0.0f;
    void prepare (double sr, float ms) { coef = msToCoef (ms, sr); }
    void snap (float v) { cur = target = v; }
    void set (float v) { target = v; }
    float next()
    {
        cur = target + (cur - target) * coef;
        if (std::fabs (cur - target) < 1.0e-7f) cur = target;
        return cur;
    }
};

// Circular delay line. read (d) returns the sample d samples before the one about to be written
// (so "read, then push" gives a delay of exactly d samples; d >= 1).
struct DelayLine
{
    std::vector<float> buf;
    int mask = 0, w = 0;

    void prepare (int maxSamples)
    {
        int size = 1;
        while (size < maxSamples + 8) size <<= 1;
        buf.assign ((size_t) size, 0.0f);
        mask = size - 1;
        w = 0;
    }
    void clear() { std::fill (buf.begin(), buf.end(), 0.0f); w = 0; }
    void push (float x) { buf[(size_t) w] = x; w = (w + 1) & mask; }
    float read (float d) const
    {
        const float pos = (float) w - d;
        const int i0 = (int) std::floor (pos);
        const float fr = pos - (float) i0;
        const float a = buf[(size_t) (i0 & mask)];
        const float b = buf[(size_t) ((i0 + 1) & mask)];
        return a + fr * (b - a);
    }
};
} // namespace vf
