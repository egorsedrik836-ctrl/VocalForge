#pragma once
#include "Dynamics.h"
#include "Effects.h"
#include "Params.h"

namespace vf
{
// The complete vocal chain:
// Trim > Mono > HPF > Gate > Hiss > Tamer > Rider > Comp1 > De-Esser > Comp2 > EQ > Saturation >
// Exciter > Doubler > Delay > Reverb > Output gain > Limiter
class VocalChain
{
public:
    struct Meters { float inDb = -100, outDb = -100, gate = 0, comp1 = 0, comp2 = 0, deess = 0, limiter = 0; };
    Meters meters;

    void prepare (double sr)
    {
        sr_ = sr;
        trim.prepare (sr, 20.0f); outG.prepare (sr, 20.0f);
        hpf.prepare (sr); gate.prepare (sr); hiss.prepare (sr); tamer.prepare (sr); rider.prepare (sr);
        comp1.prepare (sr); deess.prepare (sr); comp2.prepare (sr); eq.prepare (sr);
        sat.prepare (sr); exc.prepare (sr); dbl.prepare (sr); dly.prepare (sr); rev.prepare (sr);
        lim.prepare (sr);
        trim.snap (1.0f); outG.snap (1.0f);
        satPrev = excPrev = dblPrev = dlyPrev = revPrev = 0.0f;
    }
    int getLatencySamples() const { return lim.getLatency(); }

    void process (float* L, float* R, int n, const Params& p)
    {
        auto on = [] (float v) { return v > 0.5f; };

        // input trim + meter
        trim.set (dbToLin (p.inTrim));
        float pk = 0.0f;
        for (int i = 0; i < n; ++i)
        {
            const float g = trim.next();
            L[i] *= g; R[i] *= g;
            pk = std::max (pk, std::max (std::fabs (L[i]), std::fabs (R[i])));
        }
        meters.inDb = linToDb (pk);
        if (on (p.monoIn))
            for (int i = 0; i < n; ++i) { const float m = 0.5f * (L[i] + R[i]); L[i] = R[i] = m; }

        // ---- clean ----
        if (on (p.hpfOn)) { hpf.setParams (p.hpfFreq, (int) (p.hpfSlope + 0.5f)); hpf.process (L, R, n); } else hpf.reset();
        if (on (p.gateOn)) { gate.setParams (p.gateThr, p.gateRange, p.gateAtk, p.gateRel); gate.process (L, R, n); meters.gate = gate.getGrDb(); }
        else { gate.reset(); meters.gate = 0.0f; }
        if (on (p.hissOn)) { hiss.setParams (p.hissFreq, p.hissThr, p.hissAmt); hiss.process (L, R, n); } else hiss.reset();
        if (on (p.tamOn)) { tamer.setParams (p.tamFreq, 0.9f, p.tamThr, 3.0f, p.tamAmt, 5.0f, 80.0f); tamer.process (L, R, n, false); } else tamer.reset();
        if (on (p.ridOn)) { rider.setParams (p.ridTarget, p.ridRange, p.ridSpeed); rider.process (L, R, n); } else rider.reset();

        // ---- dynamics ----
        if (on (p.c1On)) { comp1.setParams (p.c1Thr, p.c1Ratio, p.c1Atk, p.c1Rel, p.c1Mkup); comp1.process (L, R, n); meters.comp1 = comp1.getGrDb(); }
        else { comp1.reset(); meters.comp1 = 0.0f; }
        if (on (p.dsOn)) { deess.setParams (p.dsFreq, 1.2f, p.dsThr, 6.0f, p.dsAmt, 0.5f, 30.0f); deess.process (L, R, n, on (p.dsListen)); meters.deess = deess.getGrDb(); }
        else { deess.reset(); meters.deess = 0.0f; }
        if (on (p.c2On)) { comp2.setParams (p.c2Thr, p.c2Ratio, p.c2Atk, p.c2Rel, p.c2Mkup); comp2.process (L, R, n); meters.comp2 = comp2.getGrDb(); }
        else { comp2.reset(); meters.comp2 = 0.0f; }

        // ---- color ----
        if (on (p.eqOn))
        {
            eq.setBand (0, ParametricEq::LowShelf, p.lsFreq, p.lsGain, 0.7f);
            eq.setBand (1, ParametricEq::Peak, p.lmFreq, p.lmGain, p.lmQ);
            eq.setBand (2, ParametricEq::Peak, p.prFreq, p.prGain, p.prQ);
            eq.setBand (3, ParametricEq::HighShelf, p.airFreq, p.airGain, 0.7f);
            eq.process (L, R, n);
        }
        else eq.reset();

        const float satT = on (p.satOn) ? p.satMix * 0.01f : 0.0f;
        if (satPrev > 0.0f || satT > 0.0f) sat.process (L, R, n, p.satDrive, (int) (p.satType + 0.5f), satPrev, satT); else sat.reset();
        satPrev = satT;

        const float excT = on (p.excOn) ? p.excAmt * 0.01f : 0.0f;
        if (excPrev > 0.0f || excT > 0.0f) exc.process (L, R, n, p.excFreq, excPrev, excT); else exc.reset();
        excPrev = excT;

        // ---- space ----
        const float dblT = on (p.dblOn) ? p.dblMix * 0.01f : 0.0f;
        if (dblPrev > 0.0f || dblT > 0.0f) dbl.process (L, R, n, p.dblTime, p.dblWidth * 0.01f, dblPrev, dblT); else dbl.reset();
        dblPrev = dblT;

        const float dlyT = on (p.dlyOn) ? p.dlyMix * 0.01f : 0.0f;
        if (dlyPrev > 0.0f || dlyT > 0.0f) dly.process (L, R, n, p.dlyTime, p.dlyFb * 0.01f, p.dlyTone, on (p.dlyPing), dlyPrev, dlyT); else dly.reset();
        dlyPrev = dlyT;

        const float revT = on (p.revOn) ? p.revMix * 0.01f : 0.0f;
        if (revPrev > 0.0f || revT > 0.0f) { rev.setParams (p.revDecay, p.revPre, p.revDamp, p.revLow); rev.process (L, R, n, revPrev, revT); } else rev.reset();
        revPrev = revT;

        // ---- output ----
        outG.set (dbToLin (p.outGain));
        for (int i = 0; i < n; ++i) { const float g = outG.next(); L[i] *= g; R[i] *= g; }
        if (on (p.limOn)) { lim.setParams (p.limCeil, p.limRel); lim.process (L, R, n); meters.limiter = lim.getGrDb(); }
        else
        {
            // keep latency constant even when the limiter is bypassed: run it with a ceiling of +24 dB
            lim.setParams (24.0f, p.limRel); lim.process (L, R, n); meters.limiter = 0.0f;
        }
        float opk = 0.0f;
        for (int i = 0; i < n; ++i) opk = std::max (opk, std::max (std::fabs (L[i]), std::fabs (R[i])));
        meters.outDb = linToDb (opk);
    }

private:
    double sr_ = 44100.0;
    Smoother trim, outG;
    HighPass hpf; Gate gate; HissReducer hiss; BandDynamics tamer; Rider rider;
    Compressor comp1, comp2; BandDynamics deess; ParametricEq eq;
    Saturator sat; Exciter exc; Doubler dbl; StereoDelay dly; Reverb rev;
    Limiter lim;
    float satPrev = 0, excPrev = 0, dblPrev = 0, dlyPrev = 0, revPrev = 0;
};
} // namespace vf
