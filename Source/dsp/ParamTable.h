#pragma once
// Single source of truth for every parameter.
//   XF(id, "Name", min, max, default, skewCentre (0 = linear), "unit", decimals)
//   XB(id, "Name", default)
//   XC(id, "Name", defaultIndex, "item1|item2|...")
#define VF_PARAM_LIST(XF, XB, XC) \
    /* ---- master ---- */ \
    XF(inTrim,   "Input Trim",      -24.0f, 24.0f,   0.0f,   0.0f, "dB", 1) \
    XB(monoIn,   "Mono Input",      0) \
    XF(outGain,  "Output Gain",     -24.0f, 24.0f,   0.0f,   0.0f, "dB", 1) \
    XB(limOn,    "Limiter",         1) \
    XF(limCeil,  "Limiter Ceiling", -12.0f, 0.0f,   -1.0f,   0.0f, "dB", 1) \
    XF(limRel,   "Limiter Release", 10.0f, 500.0f,  80.0f, 100.0f, "ms", 0) \
    /* ---- clean: high-pass ---- */ \
    XB(hpfOn,    "HPF",             1) \
    XF(hpfFreq,  "HPF Freq",        20.0f, 400.0f,  80.0f, 100.0f, "Hz", 0) \
    XC(hpfSlope, "HPF Slope",       1, "12 dB/oct|24 dB/oct") \
    /* ---- clean: gate ---- */ \
    XB(gateOn,   "Gate",            0) \
    XF(gateThr,  "Gate Threshold",  -80.0f, -10.0f, -50.0f,  0.0f, "dB", 1) \
    XF(gateRange,"Gate Range",      0.0f, 60.0f,    30.0f,   0.0f, "dB", 0) \
    XF(gateAtk,  "Gate Attack",     0.1f, 50.0f,     2.0f,   5.0f, "ms", 1) \
    XF(gateRel,  "Gate Release",    10.0f, 1000.0f, 150.0f, 120.0f, "ms", 0) \
    /* ---- clean: hiss reducer ---- */ \
    XB(hissOn,   "Hiss Reducer",    0) \
    XF(hissFreq, "Hiss Freq",       3000.0f, 12000.0f, 6000.0f, 6000.0f, "Hz", 0) \
    XF(hissThr,  "Hiss Threshold",  -80.0f, -20.0f, -55.0f,  0.0f, "dB", 1) \
    XF(hissAmt,  "Hiss Amount",     0.0f, 24.0f,    10.0f,   0.0f, "dB", 1) \
    /* ---- clean: resonance / mud tamer ---- */ \
    XB(tamOn,    "Tamer",           0) \
    XF(tamFreq,  "Tamer Freq",      100.0f, 800.0f, 300.0f, 300.0f, "Hz", 0) \
    XF(tamThr,   "Tamer Threshold", -60.0f, 0.0f,   -30.0f,  0.0f, "dB", 1) \
    XF(tamAmt,   "Tamer Amount",    0.0f, 18.0f,     6.0f,   0.0f, "dB", 1) \
    /* ---- clean: vocal rider ---- */ \
    XB(ridOn,    "Rider",           0) \
    XF(ridTarget,"Rider Target",    -40.0f, -6.0f,  -20.0f,  0.0f, "dB", 1) \
    XF(ridRange, "Rider Range",     0.0f, 12.0f,     6.0f,   0.0f, "dB", 1) \
    XF(ridSpeed, "Rider Speed",     100.0f, 4000.0f, 800.0f, 800.0f, "ms", 0) \
    /* ---- clean: de-esser ---- */ \
    XB(dsOn,     "De-Esser",        1) \
    XF(dsFreq,   "De-Ess Freq",     3000.0f, 12000.0f, 6500.0f, 6500.0f, "Hz", 0) \
    XF(dsThr,    "De-Ess Threshold",-60.0f, 0.0f,   -30.0f,  0.0f, "dB", 1) \
    XF(dsAmt,    "De-Ess Amount",   0.0f, 24.0f,     8.0f,   0.0f, "dB", 1) \
    XB(dsListen, "De-Ess Listen",   0) \
    /* ---- color: tone EQ ---- */ \
    XB(eqOn,     "EQ",              1) \
    XF(lsFreq,   "Low Shelf Freq",  40.0f, 400.0f,  100.0f, 120.0f, "Hz", 0) \
    XF(lsGain,   "Low Shelf Gain",  -12.0f, 12.0f,   0.0f,   0.0f, "dB", 1) \
    XF(lmFreq,   "Low-Mid Freq",    100.0f, 1000.0f, 300.0f, 300.0f, "Hz", 0) \
    XF(lmGain,   "Low-Mid Gain",    -12.0f, 12.0f,   0.0f,   0.0f, "dB", 1) \
    XF(lmQ,      "Low-Mid Q",       0.3f, 6.0f,      1.0f,   1.0f, "", 2) \
    XF(prFreq,   "Presence Freq",   1000.0f, 8000.0f, 3500.0f, 3000.0f, "Hz", 0) \
    XF(prGain,   "Presence Gain",   -12.0f, 12.0f,   0.0f,   0.0f, "dB", 1) \
    XF(prQ,      "Presence Q",      0.3f, 6.0f,      1.0f,   1.0f, "", 2) \
    XF(airFreq,  "Air Freq",        6000.0f, 16000.0f, 10000.0f, 10000.0f, "Hz", 0) \
    XF(airGain,  "Air Gain",        -12.0f, 12.0f,   0.0f,   0.0f, "dB", 1) \
    /* ---- color: compressor 1 (leveler) ---- */ \
    XB(c1On,     "Comp 1",          1) \
    XF(c1Thr,    "Comp 1 Threshold",-50.0f, 0.0f,   -18.0f,  0.0f, "dB", 1) \
    XF(c1Ratio,  "Comp 1 Ratio",    1.0f, 20.0f,     3.0f,   4.0f, ":1", 1) \
    XF(c1Atk,    "Comp 1 Attack",   0.1f, 100.0f,   20.0f,  15.0f, "ms", 1) \
    XF(c1Rel,    "Comp 1 Release",  10.0f, 1000.0f, 150.0f, 150.0f, "ms", 0) \
    XF(c1Mkup,   "Comp 1 Makeup",   0.0f, 24.0f,     0.0f,   0.0f, "dB", 1) \
    /* ---- color: compressor 2 (peak) ---- */ \
    XB(c2On,     "Comp 2",          0) \
    XF(c2Thr,    "Comp 2 Threshold",-50.0f, 0.0f,   -12.0f,  0.0f, "dB", 1) \
    XF(c2Ratio,  "Comp 2 Ratio",    1.0f, 20.0f,     4.0f,   4.0f, ":1", 1) \
    XF(c2Atk,    "Comp 2 Attack",   0.1f, 50.0f,     3.0f,   5.0f, "ms", 1) \
    XF(c2Rel,    "Comp 2 Release",  10.0f, 500.0f,  60.0f,  80.0f, "ms", 0) \
    XF(c2Mkup,   "Comp 2 Makeup",   0.0f, 24.0f,     0.0f,   0.0f, "dB", 1) \
    /* ---- color: saturation / exciter ---- */ \
    XB(satOn,    "Saturation",      0) \
    XF(satDrive, "Sat Drive",       0.0f, 24.0f,     6.0f,   0.0f, "dB", 1) \
    XC(satType,  "Sat Type",        0, "Tube|Tape|Crunch") \
    XF(satMix,   "Sat Mix",         0.0f, 100.0f,   50.0f,   0.0f, "%", 0) \
    XB(excOn,    "Exciter",         0) \
    XF(excFreq,  "Exciter Freq",    2000.0f, 10000.0f, 5000.0f, 5000.0f, "Hz", 0) \
    XF(excAmt,   "Exciter Amount",  0.0f, 100.0f,   30.0f,   0.0f, "%", 0) \
    /* ---- space: doubler ---- */ \
    XB(dblOn,    "Doubler",         0) \
    XF(dblMix,   "Doubler Mix",     0.0f, 100.0f,   30.0f,   0.0f, "%", 0) \
    XF(dblWidth, "Doubler Width",   0.0f, 200.0f,  100.0f,   0.0f, "%", 0) \
    XF(dblTime,  "Doubler Time",    6.0f, 40.0f,    14.0f,   0.0f, "ms", 1) \
    /* ---- space: delay ---- */ \
    XB(dlyOn,    "Delay",           0) \
    XC(dlySync,  "Delay Sync",      4, "Free|1/2|1/4|1/8 dotted|1/8|1/16") \
    XF(dlyTime,  "Delay Time",      20.0f, 1500.0f, 250.0f, 300.0f, "ms", 0) \
    XF(dlyFb,    "Delay Feedback",  0.0f, 90.0f,    30.0f,   0.0f, "%", 0) \
    XF(dlyTone,  "Delay Tone",      500.0f, 16000.0f, 5000.0f, 4000.0f, "Hz", 0) \
    XF(dlyMix,   "Delay Mix",       0.0f, 100.0f,   20.0f,   0.0f, "%", 0) \
    XB(dlyPing,  "Delay Ping-Pong", 1) \
    /* ---- space: reverb ---- */ \
    XB(revOn,    "Reverb",          0) \
    XF(revDecay, "Reverb Decay",    0.2f, 8.0f,      1.8f,   1.5f, "s", 1) \
    XF(revPre,   "Reverb Pre-Delay",0.0f, 200.0f,   20.0f,  40.0f, "ms", 0) \
    XF(revDamp,  "Reverb Damping",  1000.0f, 16000.0f, 7000.0f, 6000.0f, "Hz", 0) \
    XF(revLow,   "Reverb Low Cut",  20.0f, 800.0f,  200.0f, 200.0f, "Hz", 0) \
    XF(revMix,   "Reverb Mix",      0.0f, 100.0f,   15.0f,   0.0f, "%", 0)
