#pragma once
#include "Biquad.h"

namespace vf
{
class EnvFollower
{
public:
    void set (double sr, float attMs, float relMs) { aA = msToCoef (attMs, sr); aR = msToCoef (relMs, sr); }
    void reset() { env = 0.0f; }
    float process (float x)
    {
        x = std::fabs (x);
        const float a = x > env ? aA : aR;
        env = x + a * (env - x);
        return env;
    }
    float env = 0.0f;
private:
    float aA = 0.0f, aR = 0.0f;
};

// Feed-forward stereo-linked compressor with soft knee
class Compressor
{
public:
    void prepare (double sr) { sr_ = sr; reset(); }
    void reset() { grState = 0.0f; grMeter = 0.0f; }
    void setParams (float threshDb, float ratio, float attMs, float relMs, float makeupDb)
    {
        th = threshDb;
        slope = 1.0f / std::max (ratio, 1.0f) - 1.0f;
        aA = msToCoef (std::max (attMs, 0.05f), sr_);
        aR = msToCoef (std::max (relMs, 1.0f), sr_);
        makeup = dbToLin (makeupDb);
    }
    void process (float* l, float* r, int n)
    {
        float worst = 0.0f;
        for (int i = 0; i < n; ++i)
        {
            const float lvl = linToDb (std::max (std::fabs (l[i]), std::fabs (r[i])));
            const float over = lvl - th;
            float tgt;
            if (2.0f * over < -knee) tgt = 0.0f;
            else if (2.0f * std::fabs (over) <= knee) { const float t = over + 0.5f * knee; tgt = slope * t * t / (2.0f * knee); }
            else tgt = slope * over;
            const float a = tgt < grState ? aA : aR;
            grState = tgt + a * (grState - tgt);
            const float g = dbToLin (grState) * makeup;
            l[i] *= g; r[i] *= g;
            worst = std::min (worst, grState);
        }
        grMeter = -worst;
    }
    float getGrDb() const { return grMeter; }
private:
    double sr_ = 44100.0;
    float th = -20.0f, slope = -0.5f, aA = 0.0f, aR = 0.0f, makeup = 1.0f, knee = 6.0f;
    float grState = 0.0f, grMeter = 0.0f;
};

// Noise gate with hysteresis and hold
class Gate
{
public:
    void prepare (double sr) { sr_ = sr; env.set (sr, 0.05f, 15.0f); reset(); }
    void reset() { env.reset(); open = false; hold = 0; g = 1.0f; meter = 0.0f; }
    void setParams (float threshDb, float rangeDb, float attMs, float relMs)
    {
        th = threshDb;
        floorLin = dbToLin (-std::fabs (rangeDb));
        aA = msToCoef (std::max (attMs, 0.02f), sr_);
        aR = msToCoef (std::max (relMs, 1.0f), sr_);
        holdSamples = (int) (0.05 * sr_);
    }
    void process (float* l, float* r, int n)
    {
        float minG = 1.0f;
        for (int i = 0; i < n; ++i)
        {
            const float e = env.process (std::max (std::fabs (l[i]), std::fabs (r[i])));
            const float eDb = linToDb (e);
            if (eDb >= th) { open = true; hold = holdSamples; }
            else if (open)
            {
                if (eDb >= th - 4.0f) hold = holdSamples;
                else if (hold > 0) --hold;
                else open = false;
            }
            const float tgt = open ? 1.0f : floorLin;
            const float a = tgt > g ? aA : aR;
            g = tgt + a * (g - tgt);
            l[i] *= g; r[i] *= g;
            minG = std::min (minG, g);
        }
        meter = -linToDb (minG);
    }
    float getGrDb() const { return meter; }
private:
    double sr_ = 44100.0;
    EnvFollower env;
    float th = -50.0f, floorLin = 0.03f, aA = 0.0f, aR = 0.0f, g = 1.0f, meter = 0.0f;
    bool open = false;
    int hold = 0, holdSamples = 2000;
};

// Split-band dynamics: band-pass detector, reduction subtracted from the band (de-esser, resonance tamer)
class BandDynamics
{
public:
    void prepare (double sr) { sr_ = sr; lastF = -1.0f; lastQ = -1.0f; reset(); }
    void reset() { bp[0].reset(); bp[1].reset(); env.reset(); grMeter = 0.0f; }
    void setParams (float freq, float q, float threshDb, float ratio, float maxRedDb, float attMs, float relMs)
    {
        if (freq != lastF || q != lastQ)
        {
            const auto c = design::bandpass (sr_, freq, q);
            bp[0].c = c; bp[1].c = c; lastF = freq; lastQ = q;
        }
        th = threshDb;
        slope = 1.0f - 1.0f / std::max (ratio, 1.0f);
        maxRed = maxRedDb;
        env.set (sr_, attMs, relMs);
    }
    void process (float* l, float* r, int n, bool listen)
    {
        float worst = 0.0f;
        for (int i = 0; i < n; ++i)
        {
            const float bl = bp[0].process (l[i]);
            const float br = bp[1].process (r[i]);
            const float e = env.process (std::max (std::fabs (bl), std::fabs (br)));
            const float red = std::min (maxRed, std::max (0.0f, linToDb (e) - th) * slope);
            const float g = dbToLin (-red);
            if (listen) { l[i] = bl; r[i] = br; }
            else { l[i] -= (1.0f - g) * bl; r[i] -= (1.0f - g) * br; }
            worst = std::max (worst, red);
        }
        grMeter = worst;
    }
    float getGrDb() const { return grMeter; }
private:
    double sr_ = 44100.0;
    Biquad bp[2];
    EnvFollower env;
    float lastF = -1.0f, lastQ = -1.0f, th = -30.0f, slope = 0.8f, maxRed = 8.0f, grMeter = 0.0f;
};

// Reduces high-frequency hiss when the HF band falls below a threshold (band expander)
class HissReducer
{
public:
    void prepare (double sr)
    {
        sr_ = sr; lastF = -1.0f;
        env.set (sr, 1.0f, 40.0f);
        aOpen = msToCoef (3.0f, sr); aClose = msToCoef (80.0f, sr);
        reset();
    }
    void reset() { hp[0].reset(); hp[1].reset(); env.reset(); red = 0.0f; }
    void setParams (float freq, float threshDb, float amountDb)
    {
        if (freq != lastF)
        {
            const auto c = design::highpass (sr_, freq, 0.70710678);
            hp[0].c = c; hp[1].c = c; lastF = freq;
        }
        th = threshDb; amount = amountDb;
    }
    void process (float* l, float* r, int n)
    {
        for (int i = 0; i < n; ++i)
        {
            const float hl = hp[0].process (l[i]);
            const float hr = hp[1].process (r[i]);
            const float eDb = linToDb (env.process (std::max (std::fabs (hl), std::fabs (hr))));
            const float tgt = std::min (amount, std::max (0.0f, th - eDb));
            const float a = tgt > red ? aClose : aOpen;
            red = tgt + a * (red - tgt);
            const float g = dbToLin (-red);
            l[i] -= (1.0f - g) * hl;
            r[i] -= (1.0f - g) * hr;
        }
    }
private:
    double sr_ = 44100.0;
    Biquad hp[2];
    EnvFollower env;
    float lastF = -1.0f, th = -55.0f, amount = 10.0f, red = 0.0f, aOpen = 0.0f, aClose = 0.0f;
};

// Slow gain rider: keeps the phrase level near a target
class Rider
{
public:
    void prepare (double sr)
    {
        sr_ = sr; cP = (float) std::exp (-1.0 / (0.3 * sr)); reset();
    }
    void reset() { p = 0.0f; gainDb = 0.0f; }
    void setParams (float targetDb, float rangeDb, float speedMs)
    {
        target = targetDb; range = rangeDb; cG = msToCoef (std::max (speedMs, 20.0f), sr_);
    }
    void process (float* l, float* r, int n)
    {
        for (int i = 0; i < n; ++i)
        {
            const float pw = 0.5f * (l[i] * l[i] + r[i] * r[i]);
            p = pw + cP * (p - pw);
            const float lvlDb = 10.0f * std::log10 (p + 1.0e-12f);
            float desired = gainDb;
            if (lvlDb > -55.0f) desired = clampf (target - lvlDb, -range, range);
            gainDb = desired + cG * (gainDb - desired);
            const float g = dbToLin (gainDb);
            l[i] *= g; r[i] *= g;
        }
    }
    float getGainDb() const { return gainDb; }
private:
    double sr_ = 44100.0;
    float cP = 0.0f, cG = 0.0f, p = 0.0f, gainDb = 0.0f, target = -20.0f, range = 6.0f;
};

// Look-ahead peak limiter (1.5 ms), stereo linked
class Limiter
{
public:
    void prepare (double sr)
    {
        N = std::max (8, (int) std::lround (0.0015 * sr));
        dl.assign ((size_t) N, 0.0f); dr.assign ((size_t) N, 0.0f);
        req.assign ((size_t) N, 1.0f); avg.assign ((size_t) N, 1.0f);
        pos = 0; sum = (double) N; rState = 1.0f; grMeter = 0.0f; sr_ = sr;
    }
    void reset() { prepare (sr_); }
    int getLatency() const { return N - 1; }
    void setParams (float ceilDb, float relMs)
    {
        ceil = dbToLin (ceilDb);
        relCoef = msToCoef (std::max (relMs, 1.0f), sr_);
    }
    void process (float* l, float* r, int n)
    {
        float worst = 1.0f;
        for (int i = 0; i < n; ++i)
        {
            dl[(size_t) pos] = l[i]; dr[(size_t) pos] = r[i];
            const float pk = std::max (std::fabs (l[i]), std::fabs (r[i]));
            req[(size_t) pos] = pk > ceil ? ceil / pk : 1.0f;
            float m = 1.0f;
            for (int k = 0; k < N; ++k) m = std::min (m, req[(size_t) k]);
            const float rec = 1.0f - (1.0f - rState) * relCoef;
            rState = std::min (m, rec);
            sum += (double) rState - (double) avg[(size_t) pos];
            avg[(size_t) pos] = rState;
            const float g = std::min (1.0f, (float) (sum / (double) N));
            const int rd = (pos + 1) % N;
            l[i] = clampf (dl[(size_t) rd] * g, -ceil, ceil);
            r[i] = clampf (dr[(size_t) rd] * g, -ceil, ceil);
            worst = std::min (worst, g);
            pos = (pos + 1) % N;
        }
        grMeter = -linToDb (worst);
        // re-sum occasionally to avoid drift
        double s = 0.0; for (int k = 0; k < N; ++k) s += (double) avg[(size_t) k];
        sum = s;
    }
    float getGrDb() const { return grMeter; }
private:
    double sr_ = 44100.0;
    int N = 66, pos = 0;
    std::vector<float> dl, dr, req, avg;
    double sum = 0.0;
    float ceil = 0.95f, relCoef = 0.99f, rState = 1.0f, grMeter = 0.0f;
};
} // namespace vf
