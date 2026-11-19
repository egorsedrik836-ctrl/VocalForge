#pragma once
#include "Biquad.h"

namespace vf
{
// Butterworth high-pass, 12 or 24 dB/oct
class HighPass
{
public:
    void prepare (double sr) { sr_ = sr; lastF = -1.0f; lastS = -1; reset(); }
    void reset() { for (auto& st : s) for (auto& b : st) b.reset(); }
    void setParams (float f, int slope)
    {
        if (f == lastF && slope == lastS) return;
        lastF = f; lastS = slope;
        stages = slope == 0 ? 1 : 2;
        BiquadCoefs c[2];
        if (stages == 1) c[0] = design::highpass (sr_, f, 0.70710678);
        else { c[0] = design::highpass (sr_, f, 0.54119610); c[1] = design::highpass (sr_, f, 1.30656296); }
        for (int i = 0; i < stages; ++i) { s[i][0].c = c[i]; s[i][1].c = c[i]; }
    }
    void process (float* l, float* r, int n)
    {
        for (int st = 0; st < stages; ++st)
            for (int i = 0; i < n; ++i) { l[i] = s[st][0].process (l[i]); r[i] = s[st][1].process (r[i]); }
    }
private:
    double sr_ = 44100.0;
    Biquad s[2][2];
    int stages = 2, lastS = -1;
    float lastF = -1.0f;
};

// 4-band tone EQ: low shelf, low-mid bell, presence bell, air shelf
class ParametricEq
{
public:
    enum Type { LowShelf, Peak, HighShelf };
    void prepare (double sr) { sr_ = sr; for (auto& b : bands) { b.f = -1.0f; } reset(); }
    void reset() { for (auto& b : bands) { b.b[0].reset(); b.b[1].reset(); } }
    void setBand (int i, Type t, float f, float gainDb, float q)
    {
        Band& bd = bands[i];
        bd.active = std::fabs (gainDb) > 0.02f;
        if (bd.f != f || bd.g != gainDb || bd.q != q || bd.type != t)
        {
            bd.f = f; bd.g = gainDb; bd.q = q; bd.type = t;
            BiquadCoefs c;
            if (t == LowShelf) c = design::lowShelf (sr_, f, gainDb);
            else if (t == HighShelf) c = design::highShelf (sr_, f, gainDb);
            else c = design::peaking (sr_, f, gainDb, q);
            bd.b[0].c = c; bd.b[1].c = c;
        }
    }
    void process (float* l, float* r, int n)
    {
        for (auto& bd : bands)
        {
            if (! bd.active) continue;
            for (int i = 0; i < n; ++i) { l[i] = bd.b[0].process (l[i]); r[i] = bd.b[1].process (r[i]); }
        }
    }
private:
    struct Band { Biquad b[2]; Type type = Peak; float f = -1.0f, g = -999.0f, q = -1.0f; bool active = false; };
    double sr_ = 44100.0;
    Band bands[4];
};

inline float lnCosh (float x)
{
    const float a = std::fabs (x);
    return a + std::log1p (std::exp (-2.0f * a)) - 0.69314718f;
}

// Saturation with first-order antiderivative anti-aliasing (no oversampling latency)
class Saturator
{
public:
    void prepare (double sr)
    {
        lpA = (float) std::exp (-2.0 * kPi * 12000.0 / sr);
        reset();
    }
    void reset() { for (int c = 0; c < 2; ++c) { xp[c] = 0.0f; dcX[c] = 0.0f; dcY[c] = 0.0f; lp[c] = 0.0f; } }
    void process (float* l, float* r, int n, float driveDb, int type, float mix0, float mix1)
    {
        const float drive = dbToLin (driveDb);
        const float comp = std::pow (drive, -0.8f);
        float* ch[2] = { l, r };
        for (int c = 0; c < 2; ++c)
            for (int i = 0; i < n; ++i)
            {
                const float mix = mix0 + (mix1 - mix0) * ((float) i / (float) n);
                const float x = ch[c][i];
                float y = shape (x * drive, type, c) * comp;
                const float dc = y - dcX[c] + 0.995f * dcY[c];
                dcX[c] = y; dcY[c] = dc; y = dc;
                if (type == 1) { lp[c] = (1.0f - lpA) * y + lpA * lp[c]; y = lp[c]; }
                ch[c][i] = x + mix * (y - x);
            }
    }
private:
    float shape (float u, int type, int c)
    {
        const float up = xp[c];
        const float d = u - up;
        const bool small = std::fabs (d) < 1.0e-4f;
        const float m = 0.5f * (u + up);
        float y;
        if (type == 0)
        {
            const float b = 0.3f, tb = std::tanh (b);
            y = small ? std::tanh (m + b) - tb : (lnCosh (u + b) - lnCosh (up + b)) / d - tb;
        }
        else if (type == 1)
        {
            auto F = [] (float v) { const float a = std::fabs (v); return a - std::log1p (a); };
            y = small ? m / (1.0f + std::fabs (m)) : (F (u) - F (up)) / d;
        }
        else
        {
            auto F = [] (float v) { const float a = std::fabs (v); return a <= 1.0f ? 0.5f * v * v : a - 0.5f; };
            y = small ? clampf (m, -1.0f, 1.0f) : (F (u) - F (up)) / d;
        }
        xp[c] = u;
        return y;
    }
    float xp[2] = {}, dcX[2] = {}, dcY[2] = {}, lp[2] = {};
    float lpA = 0.5f;
};

// Harmonic exciter: saturate the high band, add it back
class Exciter
{
public:
    void prepare (double sr) { sr_ = sr; lastF = -1.0f; reset(); }
    void reset() { for (int c = 0; c < 2; ++c) { h1[c].reset(); h2[c].reset(); xp[c] = 0.0f; } }
    void process (float* l, float* r, int n, float freq, float amt0, float amt1)
    {
        if (freq != lastF)
        {
            const auto c = design::highpass (sr_, freq, 0.70710678);
            for (int k = 0; k < 2; ++k) { h1[k].c = c; h2[k].c = c; }
            lastF = freq;
        }
        float* ch[2] = { l, r };
        for (int c = 0; c < 2; ++c)
            for (int i = 0; i < n; ++i)
            {
                const float amt = amt0 + (amt1 - amt0) * ((float) i / (float) n);
                const float h = h1[c].process (ch[c][i]) * 4.0f;
                const float d = h - xp[c];
                const float s = std::fabs (d) < 1.0e-4f ? std::tanh (0.5f * (h + xp[c]))
                                                          : (lnCosh (h) - lnCosh (xp[c])) / d;
                xp[c] = h;
                ch[c][i] += amt * 0.5f * h2[c].process (s);
            }
    }
private:
    double sr_ = 44100.0;
    Biquad h1[2], h2[2];
    float xp[2] = {};
    float lastF = -1.0f;
};

// Stereo doubler / thickener: two slowly modulated short delays
class Doubler
{
public:
    void prepare (double sr)
    {
        sr_ = sr; dl.prepare ((int) (0.1 * sr)); dr.prepare ((int) (0.1 * sr)); reset();
    }
    void reset() { dl.clear(); dr.clear(); phA = 0.0; phB = 0.25; }
    void process (float* l, float* r, int n, float timeMs, float width, float mix0, float mix1)
    {
        const double incA = 0.37 / sr_, incB = 0.29 / sr_;
        const float baseA = timeMs * 0.001f * (float) sr_, baseB = baseA * 1.45f;
        const float depth = 0.0007f * (float) sr_;
        for (int i = 0; i < n; ++i)
        {
            const float mix = mix0 + (mix1 - mix0) * ((float) i / (float) n);
            const float mid = 0.5f * (l[i] + r[i]);
            const float dA = baseA + depth * (float) std::sin (2.0 * kPi * phA);
            const float dB = baseB + depth * (float) std::sin (2.0 * kPi * phB);
            const float wl = dl.read (dA), wr = dr.read (dB);
            dl.push (mid); dr.push (mid);
            phA += incA; if (phA >= 1.0) phA -= 1.0;
            phB += incB; if (phB >= 1.0) phB -= 1.0;
            const float m = 0.5f * (wl + wr), s = 0.5f * (wl - wr) * width;
            l[i] += 0.8f * mix * (m + s);
            r[i] += 0.8f * mix * (m - s);
        }
    }
private:
    double sr_ = 44100.0, phA = 0.0, phB = 0.25;
    DelayLine dl, dr;
};

// Stereo (ping-pong capable) delay with filtered, saturated feedback
class StereoDelay
{
public:
    void prepare (double sr)
    {
        sr_ = sr; maxD = (float) (2.0 * sr);
        dl.prepare ((int) maxD + 16); dr.prepare ((int) maxD + 16);
        timeSm.prepare (sr, 60.0f);
        reset();
    }
    void reset() { dl.clear(); dr.clear(); for (int c = 0; c < 2; ++c) { lp[c] = 0.0f; slow[c] = 0.0f; } first = true; }
    void process (float* l, float* r, int n, float timeMs, float fb, float toneHz, bool ping, float mix0, float mix1)
    {
        timeSm.set (timeMs * 0.001f * (float) sr_);
        if (first) { timeSm.snap (timeSm.target); first = false; }
        const float lpC = (float) std::exp (-2.0 * kPi * (double) toneHz / sr_);
        const float hpC = (float) std::exp (-2.0 * kPi * 120.0 / sr_);
        for (int i = 0; i < n; ++i)
        {
            const float mix = mix0 + (mix1 - mix0) * ((float) i / (float) n);
            const float d = clampf (timeSm.next(), 2.0f, maxD);
            const float yl = dl.read (d), yr = dr.read (d);
            float f[2] = { yl, yr };
            for (int c = 0; c < 2; ++c)
            {
                lp[c] = (1.0f - lpC) * f[c] + lpC * lp[c];
                slow[c] = (1.0f - hpC) * lp[c] + hpC * slow[c];
                f[c] = std::tanh (fb * (lp[c] - slow[c]));
            }
            if (ping)
            {
                const float m = 0.5f * (l[i] + r[i]);
                dl.push (m + f[1]);
                dr.push (f[0]);
            }
            else
            {
                dl.push (l[i] + f[0]);
                dr.push (r[i] + f[1]);
            }
            l[i] += mix * yl;
            r[i] += mix * yr;
        }
    }
private:
    double sr_ = 44100.0;
    float maxD = 88200.0f;
    DelayLine dl, dr;
    Smoother timeSm;
    float lp[2] = {}, slow[2] = {};
    bool first = true;
};

// 8-line feedback delay network reverb (Householder feedback matrix)
class Reverb
{
public:
    static constexpr int N = 8;
    void prepare (double sr)
    {
        sr_ = sr;
        static const float ms[N] = { 29.65f, 37.15f, 41.10f, 43.75f, 52.97f, 59.30f, 61.30f, 67.70f };
        for (int i = 0; i < N; ++i)
        {
            len[i] = ms[i] * 0.001f * (float) sr;
            line[i].prepare ((int) len[i] + (int) (0.002 * sr) + 16);
            lfoInc[i] = (0.11 + 0.037 * i) / sr;
        }
        pre.prepare ((int) (0.25 * sr));
        const float apMs[2] = { 5.3f, 8.1f };
        for (int i = 0; i < 2; ++i) { apLen[i] = apMs[i] * 0.001f * (float) sr; ap[i].prepare ((int) apLen[i] + 8); }
        lastLow = -1.0f;
        reset();
    }
    void reset()
    {
        for (int i = 0; i < N; ++i) { line[i].clear(); lp[i] = 0.0f; lfo[i] = (double) i / N; }
        pre.clear(); for (auto& a : ap) a.clear();
        hp.reset();
    }
    void setParams (float decaySec, float preMs, float dampHz, float lowCutHz)
    {
        const float rt = std::max (decaySec, 0.1f);
        for (int i = 0; i < N; ++i) g[i] = std::pow (10.0f, -3.0f * len[i] / (rt * (float) sr_));
        dampC = (float) std::exp (-2.0 * kPi * (double) dampHz / sr_);
        preSamples = std::max (1.0f, preMs * 0.001f * (float) sr_);
        if (lowCutHz != lastLow) { hp.c = design::highpass (sr_, lowCutHz, 0.70710678); lastLow = lowCutHz; }
    }
    void process (float* l, float* r, int n, float mix0, float mix1)
    {
        static const float sgn[N] = { 1, 1, -1, -1, 1, -1, 1, -1 };
        const float depth = 0.0004f * (float) sr_;
        for (int k = 0; k < n; ++k)
        {
            const float mix = mix0 + (mix1 - mix0) * ((float) k / (float) n);
            float x = 0.5f * (l[k] + r[k]);
            const float pd = pre.read (preSamples);
            pre.push (x);
            x = hp.process (pd);
            for (int a = 0; a < 2; ++a)
            {
                const float d = ap[a].read (apLen[a]);
                const float w = x + 0.6f * d;
                ap[a].push (w);
                x = d - 0.6f * w;
            }
            float d[N]; float sum = 0.0f;
            for (int i = 0; i < N; ++i)
            {
                d[i] = line[i].read (len[i] + depth * (float) std::sin (2.0 * kPi * lfo[i]));
                lfo[i] += lfoInc[i]; if (lfo[i] >= 1.0) lfo[i] -= 1.0;
                sum += d[i];
            }
            const float k2 = 2.0f / (float) N * sum;
            float wl = 0.0f, wr = 0.0f;
            for (int i = 0; i < N; ++i)
            {
                float fbv = (d[i] - k2) * g[i];
                lp[i] = (1.0f - dampC) * fbv + dampC * lp[i];
                if (std::fabs (lp[i]) < 1.0e-20f) lp[i] = 0.0f;
                line[i].push (0.35f * sgn[i] * x + lp[i]);
                if (i & 1) wr += sgn[i] * d[i]; else wl += sgn[i] * d[i];
            }
            l[k] += mix * wl;
            r[k] += mix * wr;
        }
    }
private:
    double sr_ = 44100.0;
    float len[N] = {}, g[N] = {}, lp[N] = {}, dampC = 0.5f, preSamples = 100.0f, apLen[2] = {}, lastLow = -1.0f;
    double lfo[N] = {}, lfoInc[N] = {};
    DelayLine line[N], pre, ap[2];
    Biquad hp;
};
} // namespace vf
