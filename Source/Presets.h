#pragma once
#include <vector>

namespace vf
{
struct PresetValue { const char* id; float value; };
struct Preset { const char* name; std::vector<PresetValue> values; };

// Every preset starts from the default state; only differences are listed.
// dlySync: 0 Free, 1 1/2, 2 1/4, 3 1/8 dotted, 4 1/8, 5 1/16
inline const std::vector<Preset>& getPresets()
{
    static const std::vector<Preset> presets =
    {
        { "Init (all defaults)", {} },

        { "Clean Pop Vocal", {
            { "hpfFreq", 90 }, { "gateOn", 1 }, { "gateThr", -52 }, { "gateRange", 18 },
            { "dsOn", 1 }, { "dsThr", -28 }, { "dsAmt", 8 },
            { "c1On", 1 }, { "c1Thr", -20 }, { "c1Ratio", 3 }, { "c1Mkup", 3 },
            { "c2On", 1 }, { "c2Thr", -10 }, { "c2Ratio", 3 },
            { "lmGain", -2.5f }, { "prGain", 2.0f }, { "airGain", 3.0f },
            { "satOn", 1 }, { "satDrive", 4 }, { "satMix", 30 },
            { "excOn", 1 }, { "excAmt", 20 },
            { "dblOn", 1 }, { "dblMix", 20 },
            { "revOn", 1 }, { "revDecay", 1.6f }, { "revMix", 14 },
            { "dlyOn", 1 }, { "dlySync", 4 }, { "dlyMix", 12 } } },

        { "Rap Upfront", {
            { "hpfFreq", 100 }, { "gateOn", 1 }, { "gateThr", -45 }, { "gateRange", 30 }, { "gateAtk", 1 }, { "gateRel", 100 },
            { "tamOn", 1 }, { "tamFreq", 300 }, { "tamThr", -28 }, { "tamAmt", 4 },
            { "dsOn", 1 }, { "dsThr", -26 }, { "dsAmt", 10 },
            { "c1On", 1 }, { "c1Thr", -22 }, { "c1Ratio", 4 }, { "c1Atk", 8 }, { "c1Mkup", 4 },
            { "c2On", 1 }, { "c2Thr", -14 }, { "c2Ratio", 6 }, { "c2Atk", 1 }, { "c2Rel", 40 }, { "c2Mkup", 2 },
            { "lsGain", 1.5f }, { "lmGain", -2.0f }, { "prGain", 2.5f }, { "airGain", 2.0f },
            { "satOn", 1 }, { "satDrive", 8 }, { "satType", 1 }, { "satMix", 40 },
            { "excOn", 1 }, { "excAmt", 25 },
            { "dblOn", 1 }, { "dblMix", 35 }, { "dblWidth", 120 }, { "dblTime", 12 },
            { "dlyOn", 1 }, { "dlySync", 4 }, { "dlyMix", 10 },
            { "revOn", 1 }, { "revDecay", 1.0f }, { "revPre", 10 }, { "revMix", 8 } } },

        { "Airy Female", {
            { "hpfFreq", 120 },
            { "dsOn", 1 }, { "dsFreq", 7500 }, { "dsThr", -30 }, { "dsAmt", 10 },
            { "c1On", 1 }, { "c1Thr", -18 }, { "c1Ratio", 2.5f }, { "c1Atk", 25 }, { "c1Mkup", 2 },
            { "lmFreq", 350 }, { "lmGain", -2.0f }, { "prFreq", 4000 }, { "prGain", 1.5f },
            { "airFreq", 12000 }, { "airGain", 5.0f },
            { "excOn", 1 }, { "excFreq", 7000 }, { "excAmt", 35 },
            { "satOn", 1 }, { "satDrive", 3 }, { "satType", 1 }, { "satMix", 30 },
            { "dblOn", 1 }, { "dblMix", 25 }, { "dblTime", 18 },
            { "revOn", 1 }, { "revDecay", 2.4f }, { "revPre", 30 }, { "revMix", 20 },
            { "dlyOn", 1 }, { "dlySync", 2 }, { "dlyMix", 14 } } },

        { "Warm Male", {
            { "hpfFreq", 70 }, { "gateOn", 1 }, { "gateThr", -50 }, { "gateRange", 20 },
            { "tamOn", 1 }, { "tamFreq", 250 }, { "tamThr", -28 }, { "tamAmt", 5 },
            { "dsOn", 1 }, { "dsFreq", 6000 },
            { "c1On", 1 }, { "c1Thr", -20 }, { "c1Ratio", 3.5f }, { "c1Mkup", 3 },
            { "lsFreq", 120 }, { "lsGain", 2.0f }, { "lmFreq", 280 }, { "lmGain", -1.5f }, { "prFreq", 3000 }, { "prGain", 1.5f }, { "airGain", 2.0f },
            { "satOn", 1 }, { "satDrive", 8 }, { "satType", 0 }, { "satMix", 45 },
            { "excOn", 1 }, { "excAmt", 15 },
            { "revOn", 1 }, { "revDecay", 1.4f }, { "revMix", 10 } } },

        { "Podcast / Voiceover", {
            { "hpfFreq", 90 }, { "gateOn", 1 }, { "gateThr", -48 }, { "gateRange", 24 },
            { "hissOn", 1 }, { "ridOn", 1 }, { "ridTarget", -20 }, { "ridRange", 6 }, { "ridSpeed", 700 },
            { "dsOn", 1 }, { "dsThr", -30 }, { "dsAmt", 6 },
            { "c1On", 1 }, { "c1Thr", -22 }, { "c1Ratio", 3 }, { "c1Mkup", 4 },
            { "c2On", 1 }, { "c2Thr", -14 }, { "c2Ratio", 3 },
            { "lmFreq", 250 }, { "lmGain", -2.0f }, { "prFreq", 3000 }, { "prGain", 2.5f }, { "airGain", 1.0f } } },

        { "Lo-Fi Radio", {
            { "hpfFreq", 300 },
            { "dsOn", 0 }, { "c1On", 1 }, { "c1Thr", -25 }, { "c1Ratio", 6 }, { "c1Mkup", 6 },
            { "prFreq", 1800 }, { "prGain", 5.0f }, { "airFreq", 6000 }, { "airGain", -10.0f },
            { "satOn", 1 }, { "satDrive", 14 }, { "satType", 2 }, { "satMix", 70 },
            { "dlyOn", 1 }, { "dlySync", 4 }, { "dlyMix", 15 },
            { "revOn", 1 }, { "revDecay", 1.0f }, { "revMix", 10 } } },
    };
    return presets;
}
} // namespace vf
