// Offline sanity tests for the JUCE-free DSP core. Build: g++ -std=c++17 -O2 -I../Source/dsp dsp_test.cpp -o dsp_test
#include "VocalChain.h"
#include "../Source/Presets.h"
#include <map>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <random>

using namespace vf;
static int fails = 0;
#define CHECK(cond, ...) do { if (cond) std::printf("  PASS  "); else { std::printf("  FAIL  "); ++fails; } std::printf(__VA_ARGS__); std::printf("\n"); } while (0)

static const double SR = 48000.0;

static Params offAll()
{
    Params p;
    p.hpfOn = p.gateOn = p.hissOn = p.tamOn = p.ridOn = p.dsOn = p.eqOn = p.c1On = p.c2On = 0;
    p.satOn = p.excOn = p.dblOn = p.dlyOn = p.revOn = p.limOn = 0;
    return p;
}
static std::vector<float> sine (double f, double amp, int n)
{
    std::vector<float> v ((size_t) n);
    for (int i = 0; i < n; ++i) v[(size_t) i] = (float) (amp * std::sin (2.0 * kPi * f * i / SR));
    return v;
}
static double rmsRange (const std::vector<float>& v, int a, int b)
{
    double s = 0; for (int i = a; i < b; ++i) s += (double) v[(size_t) i] * v[(size_t) i];
    return std::sqrt (s / std::max (1, b - a));
}
static double db (double x) { return 20.0 * std::log10 (std::max (x, 1e-12)); }

// run mono-in-both-channels signal in blocks of 512
static std::vector<float> run (VocalChain& c, const Params& p, std::vector<float> in)
{
    std::vector<float> L = in, R = in;
    for (size_t i = 0; i < L.size(); i += 512)
    {
        const int n = (int) std::min<size_t> (512, L.size() - i);
        c.process (L.data() + i, R.data() + i, n, p);
    }
    return L;
}
static bool finite (const std::vector<float>& v) { for (float x : v) if (! std::isfinite (x)) return false; return true; }

int main()
{
    std::printf ("VocalForge DSP tests @ %.0f Hz\n", SR);
    std::mt19937 rng (1); std::normal_distribution<float> nd (0.0f, 1.0f);

    { std::printf ("[1] bit-transparent when everything is off (except fixed limiter latency)\n");
      VocalChain c; c.prepare (SR); Params p = offAll(); auto in = sine (440, 0.3, 24000);
      auto out = run (c, p, in); const int lat = c.getLatencySamples();
      double maxErr = 0; for (int i = lat + 100; i < 24000; ++i) maxErr = std::max (maxErr, (double) std::fabs (out[(size_t) i] - in[(size_t) (i - lat)]));
      CHECK (maxErr < 1e-4, "max error vs delayed input = %.2e (latency %d samples)", maxErr, lat); }

    { std::printf ("[2] high-pass 24 dB/oct @100 Hz\n");
      Params p = offAll(); p.hpfOn = 1; p.hpfFreq = 100; p.hpfSlope = 1;
      VocalChain c; c.prepare (SR); auto o50 = run (c, p, sine (50, 0.5, 48000));
      VocalChain c2; c2.prepare (SR); auto o1k = run (c2, p, sine (1000, 0.5, 48000));
      const double a50 = db (rmsRange (o50, 24000, 48000) / (0.5 / std::sqrt (2.0)));
      const double a1k = db (rmsRange (o1k, 24000, 48000) / (0.5 / std::sqrt (2.0)));
      CHECK (a50 < -20.0, "50 Hz attenuated by %.1f dB (expect ~ -24)", a50);
      CHECK (std::fabs (a1k) < 0.2, "1 kHz passes (%.2f dB)", a1k); }

    { std::printf ("[3] gate closes on noise floor, passes loud signal\n");
      Params p = offAll(); p.gateOn = 1; p.gateThr = -45; p.gateRange = 40; p.gateRel = 80;
      std::vector<float> in (96000);
      for (int i = 0; i < 48000; ++i) in[(size_t) i] = 0.0015f * nd (rng);           // ~ -56 dB noise
      auto s = sine (220, 0.3, 48000); for (int i = 0; i < 48000; ++i) in[(size_t) (48000 + i)] = s[(size_t) i];
      VocalChain c; c.prepare (SR); auto out = run (c, p, in);
      const double nIn = db (rmsRange (in, 24000, 48000)), nOut = db (rmsRange (out, 24000, 48000));
      const double sIn = db (rmsRange (in, 60000, 96000)), sOut = db (rmsRange (out, 60000, 96000));
      CHECK (nIn - nOut > 25.0, "noise reduced by %.1f dB", nIn - nOut);
      CHECK (std::fabs (sIn - sOut) < 0.3, "signal untouched (%.2f dB)", sOut - sIn); }

    { std::printf ("[4] compressor static curve (thr -20, ratio 4, 1 kHz @ -6 dBFS peak)\n");
      Params p = offAll(); p.c1On = 1; p.c1Thr = -20; p.c1Ratio = 4; p.c1Atk = 1; p.c1Rel = 50; p.c1Mkup = 0;
      VocalChain c; c.prepare (SR); auto in = sine (1000, 0.5012, 48000); auto out = run (c, p, in);
      double pkIn = 0, pkOut = 0; for (int i = 36000; i < 48000; ++i) { pkIn = std::max (pkIn, (double) std::fabs (in[(size_t) i])); pkOut = std::max (pkOut, (double) std::fabs (out[(size_t) i])); }
      const double gr = db (pkIn) - db (pkOut); // expected 14 * 0.75 = 10.5
      CHECK (std::fabs (gr - 10.5) < 1.0, "gain reduction %.2f dB (expect 10.5), meter %.2f", gr, c.meters.comp1); }

    { std::printf ("[5] de-esser reduces 7 kHz, leaves 200 Hz alone\n");
      Params base = offAll(); Params ds = base; ds.dsOn = 1; ds.dsFreq = 7000; ds.dsThr = -30; ds.dsAmt = 12;
      VocalChain a; a.prepare (SR); VocalChain b; b.prepare (SR);
      auto x7 = sine (7000, 0.3, 48000);
      const double r0 = rmsRange (run (a, base, x7), 24000, 48000), r1 = rmsRange (run (b, ds, x7), 24000, 48000);
      VocalChain a2; a2.prepare (SR); VocalChain b2; b2.prepare (SR);
      auto x2 = sine (200, 0.3, 48000);
      const double q0 = rmsRange (run (a2, base, x2), 24000, 48000), q1 = rmsRange (run (b2, ds, x2), 24000, 48000);
      CHECK (db (r0) - db (r1) > 8.0, "7 kHz reduced by %.1f dB (max 12)", db (r0) - db (r1));
      CHECK (std::fabs (db (q0) - db (q1)) < 0.3, "200 Hz changed by %.2f dB", db (q1) - db (q0)); }

    { std::printf ("[6] EQ presence bell +6 dB @3.5 kHz, low shelf, air shelf\n");
      Params p = offAll(); p.eqOn = 1; p.prGain = 6; p.prFreq = 3500; p.prQ = 1.0f; p.airGain = 6; p.airFreq = 10000; p.lsGain = -6; p.lsFreq = 100;
      auto meas = [&] (double f) { VocalChain c; c.prepare (SR); auto o = run (c, p, sine (f, 0.1, 48000)); return db (rmsRange (o, 24000, 48000) / (0.1 / std::sqrt (2.0))); };
      const double g35 = meas (3500), g20 = meas (20000), g40 = meas (40);
      CHECK (std::fabs (g35 - 6.0) < 0.6, "3.5 kHz gain %.2f dB (expect ~+6)", g35);
      CHECK (g20 > 5.0, "20 kHz gain %.2f dB (air shelf, expect ~+6)", g20);
      CHECK (g40 < -4.5, "40 Hz gain %.2f dB (low shelf, expect ~-6)", g40); }

    { std::printf ("[7] limiter never exceeds ceiling\n");
      Params p = offAll(); p.limOn = 1; p.limCeil = -3; p.limRel = 80;
      std::vector<float> in (96000); for (auto& x : in) x = 1.5f * nd (rng) * 0.5f;
      VocalChain c; c.prepare (SR); auto out = run (c, p, in);
      double pk = 0; for (float x : out) pk = std::max (pk, (double) std::fabs (x));
      CHECK (db (pk) <= -3.0 + 0.01, "peak %.3f dBFS (ceiling -3.0)", db (pk));
      auto s = sine (1000, 0.2, 48000); VocalChain c2; c2.prepare (SR); auto o2 = run (c2, p, s);
      CHECK (std::fabs (db (rmsRange (o2, 24000, 48000)) - db (rmsRange (s, 24000, 48000))) < 0.05, "quiet signal below ceiling untouched"); }

    { std::printf ("[8] reverb: finite, decays, roughly the requested RT60\n");
      Params p = offAll(); p.revOn = 1; p.revMix = 100; p.revDecay = 1.5f; p.revPre = 0; p.revDamp = 16000; p.revLow = 20;
      std::vector<float> in (48000 * 5, 0.0f); for (int i = 0; i < 2000; ++i) in[(size_t) i] = 0.3f * nd (rng);
      VocalChain c; c.prepare (SR); auto out = run (c, p, in);
      CHECK (finite (out), "no NaN / inf");
      auto win = [&] (double t0, double t1) { return db (rmsRange (out, (int) (t0 * SR), (int) (t1 * SR))); };
      const double e1 = win (0.3, 0.4), e2 = win (1.3, 1.4);   // 1.0 s apart
      const double rt60 = 60.0 / ((e1 - e2) / 1.0);
      CHECK (rt60 > 0.9 && rt60 < 2.4, "measured RT60 ~ %.2f s (requested 1.5)", rt60);
      CHECK (win (4.5, 5.0) < win (0.3, 0.4) - 40.0, "tail %.1f dB after 4.5 s", win (4.5, 5.0) - win (0.3, 0.4));
      // loudness sanity: noise in (rms 0.1) -> wet rms
      Params p2 = p; p2.revDecay = 2.0f; std::vector<float> nz (48000 * 3); for (auto& x : nz) x = 0.1f * nd (rng);
      VocalChain c2; c2.prepare (SR); auto o2 = run (c2, p2, nz);
      std::printf ("        (info) wet/dry loudness with 100%% mix: %.1f dB\n", db (rmsRange (o2, 48000, 144000)) - db (rmsRange (nz, 48000, 144000))); }

    { std::printf ("[9] delay echo lands at the requested time\n");
      Params p = offAll(); p.dlyOn = 1; p.dlyMix = 100; p.dlyFb = 0; p.dlyPing = 0; p.dlyTime = 100; p.dlyTone = 16000;
      std::vector<float> in (48000, 0.0f); in[1000] = 0.5f;
      VocalChain c; c.prepare (SR); auto out = run (c, p, in);
      int best = 0; for (int i = 2000; i < 48000; ++i) if (std::fabs (out[(size_t) i]) > std::fabs (out[(size_t) best])) best = i;
      const int expect = 1000 + 4800 + c.getLatencySamples();
      CHECK (std::abs (best - expect) <= 2, "echo at sample %d (expect %d incl. limiter latency), level %.2f", best, expect, out[(size_t) best]); }

    { std::printf ("[10] saturation + exciter + doubler: finite, bounded, audible\n");
      Params p = offAll(); p.satOn = 1; p.satDrive = 18; p.satMix = 100; p.excOn = 1; p.excAmt = 100; p.dblOn = 1; p.dblMix = 100;
      for (int t = 0; t < 3; ++t) { p.satType = (float) t; VocalChain c; c.prepare (SR); auto o = run (c, p, sine (440, 0.5, 48000));
        double pk = 0; for (float x : o) pk = std::max (pk, (double) std::fabs (x));
        CHECK (finite (o) && pk < 3.0 && pk > 0.05, "type %d: peak %.2f", t, pk); } }

    { std::printf ("[11] stress: all modules on, extreme settings, white noise, odd block sizes\n");
      Params p; p.hpfOn = p.gateOn = p.hissOn = p.tamOn = p.ridOn = p.dsOn = p.eqOn = p.c1On = p.c2On = p.satOn = p.excOn = p.dblOn = p.dlyOn = p.revOn = p.limOn = 1;
      p.lsGain = 12; p.lmGain = -12; p.prGain = 12; p.airGain = 12; p.satDrive = 24; p.dlyFb = 90; p.revDecay = 8; p.c1Ratio = 20; p.c2Ratio = 20; p.limCeil = -6; p.inTrim = 12; p.outGain = 12;
      VocalChain c; c.prepare (SR);
      std::vector<float> L (48000 * 4), R (48000 * 4); for (size_t i = 0; i < L.size(); ++i) { L[i] = 0.5f * nd (rng); R[i] = 0.5f * nd (rng); }
      size_t i = 0; int bs[5] = { 1, 7, 64, 333, 1024 }; int k = 0;
      while (i < L.size()) { int n = (int) std::min<size_t> ((size_t) bs[k++ % 5], L.size() - i); c.process (L.data() + i, R.data() + i, n, p); i += (size_t) n; }
      double pk = 0; bool ok = true; for (size_t j = 0; j < L.size(); ++j) { ok &= std::isfinite (L[j]) && std::isfinite (R[j]); pk = std::max (pk, (double) std::max (std::fabs (L[j]), std::fabs (R[j]))); }
      CHECK (ok, "no NaN / inf"); CHECK (db (pk) <= -6.0 + 0.01, "peak %.2f dBFS <= ceiling -6", db (pk));
      // then silence: everything must decay to (near) zero
      std::vector<float> sL (48000 * 20, 0.0f), sR = sL; i = 0; while (i < sL.size()) { c.process (sL.data() + i, sR.data() + i, 512, p); i += 512; }
      CHECK (rmsRange (sL, sL.size() - 48000, sL.size()) < 1e-3, "tail with feedback at 90%% still decays: rms %.2e (reverb 8 s, delay fb 90%%)", rmsRange (sL, sL.size() - 48000, sL.size())); }

    { std::printf ("[12] other sample rates\n");
      for (double sr : { 44100.0, 88200.0, 96000.0 })
      { Params p; p.revOn = p.satOn = p.dlyOn = p.dblOn = p.c2On = 1; VocalChain c; c.prepare (sr);
        std::vector<float> L (48000), R; for (auto& x : L) x = 0.3f * nd (rng); R = L;
        for (size_t i = 0; i < L.size(); i += 480) c.process (L.data() + i, R.data() + i, 480, p);
        CHECK (finite (L) && finite (R), "%.0f Hz ok, limiter latency %d samples", sr, c.getLatencySamples()); } }


    { std::printf ("[13] presets reference real parameters with legal values; every preset renders cleanly\n");
      struct Info { float mn, mx; };
      std::map<std::string, Info> reg;
#define R_F(id, name, mn, mx, def, ctr, unit, dec) reg[#id] = { (float) (mn), (float) (mx) };
#define R_B(id, name, def) reg[#id] = { 0.0f, 1.0f };
#define R_C(id, name, def, items) reg[#id] = { 0.0f, 99.0f };
      VF_PARAM_LIST (R_F, R_B, R_C)
      std::printf ("        (info) %zu parameters\n", reg.size());
      bool idsOk = true, rangeOk = true; int audioOk = 0;
      for (const auto& pr : getPresets())
      {
          Params p;
          for (const auto& v : pr.values)
          {
              auto it = reg.find (v.id);
              if (it == reg.end()) { idsOk = false; std::printf ("        unknown id %s in preset %s\n", v.id, pr.name); continue; }
              if (v.value < it->second.mn || v.value > it->second.mx) { rangeOk = false; std::printf ("        out of range %s=%g in %s\n", v.id, v.value, pr.name); }
              float* dst = nullptr; const std::string key = v.id;
#define S_F(id, name, mn, mx, def, ctr, unit, dec) if (key == #id) dst = &p.id;
#define S_B(id, name, def) if (key == #id) dst = &p.id;
#define S_C(id, name, def, items) if (key == #id) dst = &p.id;
              VF_PARAM_LIST (S_F, S_B, S_C)
              if (dst) *dst = v.value;
          }
          VocalChain c; c.prepare (SR);
          std::vector<float> L (96000), R; for (size_t i = 0; i < L.size(); ++i) L[i] = (float) (0.3 * std::sin (2.0 * kPi * 220.0 * (double) i / SR) * (0.5 + 0.5 * std::sin (2.0 * kPi * 2.0 * (double) i / SR)) + 0.002 * nd (rng));
          R = L;
          for (size_t i = 0; i < L.size(); i += 512) c.process (L.data() + i, R.data() + i, (int) std::min<size_t> (512, L.size() - i), p);
          double pk = 0; bool fin = true; for (float x : L) { fin &= std::isfinite (x); pk = std::max (pk, (double) std::fabs (x)); }
          if (fin && pk < 1.0 && pk > 0.01) ++audioOk;
          else std::printf ("        preset %s: finite=%d peak=%.3f\n", pr.name, (int) fin, pk);
      }
      CHECK (idsOk, "all preset IDs exist"); CHECK (rangeOk, "all preset values within range");
      CHECK (audioOk == (int) getPresets().size(), "%d/%zu presets render finite, non-silent, below 0 dBFS", audioOk, getPresets().size()); }

    std::printf ("\n%s (%d failure%s)\n", fails ? "SOME TESTS FAILED" : "ALL TESTS PASSED", fails, fails == 1 ? "" : "s");
    return fails ? 1 : 0;
}
