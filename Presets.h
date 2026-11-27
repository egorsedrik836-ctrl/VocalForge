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


        // ================= Artist-style presets (approximations from each artist's general vocal character) =================
        { "Artist: Gone.Fludd style (airy cloud)", {
            { "hpfFreq", 110 }, { "dsThr", -30 }, { "dsAmt", 9 }, { "c1Thr", -18 }, { "c1Ratio", 2.5f },
            { "c1Atk", 25 }, { "c1Mkup", 2 }, { "lmFreq", 320 }, { "lmGain", -2.5f }, { "prGain", 0.5f },
            { "airFreq", 9000 }, { "airGain", 2 }, { "satOn", 1 }, { "satDrive", 8 }, { "satType", 1 },
            { "satMix", 40 }, { "excOn", 1 }, { "excFreq", 7000 }, { "excAmt", 10 }, { "dblOn", 1 }, { "dblMix", 50 },
            { "dblWidth", 200 }, { "dblTime", 22 }, { "dlyOn", 1 }, { "dlySync", 2 }, { "dlyFb", 48 },
            { "dlyTone", 4000 }, { "dlyMix", 26 }, { "revOn", 1 }, { "revDecay", 4.2f }, { "revPre", 70 },
            { "revDamp", 6000 }, { "revLow", 250 }, { "revMix", 36 } } },

        { "Artist: Pharaoh style (dark cloud)", {
            { "hpfFreq", 85 }, { "gateOn", 1 }, { "gateThr", -50 }, { "gateRange", 20 }, { "tamOn", 1 },
            { "tamFreq", 280 }, { "tamThr", -28 }, { "tamAmt", 4 }, { "dsThr", -28 }, { "dsAmt", 7 }, { "c1Thr", -24 },
            { "c1Ratio", 4 }, { "c1Atk", 12 }, { "c1Mkup", 5 }, { "lsFreq", 110 }, { "lsGain", 4 }, { "lmGain", -1 },
            { "prFreq", 2800 }, { "prGain", 1 }, { "airFreq", 8000 }, { "airGain", -6 }, { "satOn", 1 },
            { "satDrive", 11 }, { "satType", 0 }, { "satMix", 55 }, { "dblOn", 1 }, { "dblMix", 22 },
            { "dblTime", 20 }, { "dlyOn", 1 }, { "dlySync", 2 }, { "dlyFb", 45 }, { "dlyTone", 2200 },
            { "dlyMix", 20 }, { "revOn", 1 }, { "revDecay", 5.5f }, { "revPre", 60 }, { "revDamp", 3000 },
            { "revLow", 220 }, { "revMix", 36 } } },

        { "Artist: Lil Pump style (loud, crunchy)", {
            { "hpfFreq", 150 }, { "dsThr", -26 }, { "dsAmt", 8 }, { "c1Thr", -28 }, { "c1Ratio", 8 }, { "c1Atk", 4 },
            { "c1Rel", 80 }, { "c1Mkup", 6 }, { "c2On", 1 }, { "c2Thr", -18 }, { "c2Ratio", 12 }, { "c2Atk", 0.8f },
            { "c2Rel", 35 }, { "c2Mkup", 4 }, { "lmFreq", 500 }, { "lmGain", -2.5f }, { "prFreq", 2200 },
            { "prGain", 5 }, { "airGain", 0.5f }, { "satOn", 1 }, { "satDrive", 22 }, { "satType", 2 },
            { "satMix", 60 }, { "excOn", 1 }, { "excAmt", 15 }, { "dblOn", 1 }, { "dblMix", 25 }, { "dblWidth", 70 },
            { "dblTime", 9 }, { "dlyOn", 1 }, { "dlySync", 5 }, { "dlyFb", 12 }, { "dlyMix", 7 }, { "dlyPing", 0 },
            { "revOn", 1 }, { "revDecay", 0.5f }, { "revPre", 3 }, { "revMix", 5 }, { "limCeil", -0.5f },
            { "outGain", 2.5f } } },

        { "Artist: madk1d style (blown-out, digital)", {
            { "hpfFreq", 200 }, { "gateOn", 1 }, { "gateThr", -48 }, { "gateRange", 26 }, { "gateAtk", 0.5f },
            { "gateRel", 80 }, { "dsFreq", 7500 }, { "dsThr", -30 }, { "dsAmt", 12 }, { "c1Thr", -26 },
            { "c1Ratio", 6 }, { "c1Atk", 4 }, { "c1Mkup", 4 }, { "c2On", 1 }, { "c2Thr", -16 }, { "c2Ratio", 10 },
            { "c2Atk", 1 }, { "c2Rel", 40 }, { "c2Mkup", 3 }, { "lsGain", -3 }, { "lmFreq", 400 }, { "lmGain", -4 },
            { "prFreq", 3000 }, { "prGain", 4 }, { "airFreq", 13000 }, { "airGain", 5 }, { "satOn", 1 },
            { "satDrive", 20 }, { "satType", 3 }, { "satMix", 90 }, { "excOn", 1 }, { "excFreq", 4500 },
            { "excAmt", 45 }, { "dblOn", 1 }, { "dblMix", 35 }, { "dblWidth", 170 }, { "dblTime", 20 }, { "dlyOn", 1 },
            { "dlySync", 5 }, { "dlyFb", 40 }, { "dlyTone", 7000 }, { "dlyMix", 16 }, { "revOn", 1 },
            { "revDecay", 1.1f }, { "revPre", 8 }, { "revDamp", 9000 }, { "revMix", 12 }, { "outGain", 2.5f } } },

        { "Artist: LSP style (dry, warm, natural)", {
            { "hpfFreq", 90 }, { "gateOn", 1 }, { "gateThr", -52 }, { "gateRange", 14 }, { "dsThr", -29 },
            { "dsAmt", 9 }, { "c1Thr", -19 }, { "c1Ratio", 2.5f }, { "c1Atk", 18 }, { "c1Mkup", 3 }, { "lsGain", 3 },
            { "lmFreq", 250 }, { "lmGain", 1 }, { "prGain", 0 }, { "airGain", -1 }, { "satOn", 1 }, { "satDrive", 4 },
            { "satType", 0 }, { "satMix", 25 }, { "dlyOn", 1 }, { "dlySync", 4 }, { "dlyFb", 20 }, { "dlyMix", 5 },
            { "revOn", 1 }, { "revDecay", 0.9f }, { "revPre", 10 }, { "revMix", 6 }, { "airFreq", 9000 },
            { "outGain", -1.5f } } },

        { "Artist: Temny Princ style (dark, dense)", {
            { "hpfFreq", 90 }, { "gateOn", 1 }, { "gateThr", -48 }, { "gateRange", 22 }, { "tamOn", 1 },
            { "tamFreq", 300 }, { "tamThr", -28 }, { "tamAmt", 5 }, { "dsThr", -28 }, { "dsAmt", 8 }, { "c1Thr", -24 },
            { "c1Ratio", 6 }, { "c1Atk", 8 }, { "c1Mkup", 5 }, { "c2On", 1 }, { "c2Thr", -16 }, { "c2Ratio", 8 },
            { "c2Atk", 1.5f }, { "c2Rel", 50 }, { "c2Mkup", 3 }, { "lsFreq", 100 }, { "lsGain", 3.5f },
            { "lmFreq", 320 }, { "lmGain", -2 }, { "prFreq", 2500 }, { "prGain", 2.5f }, { "airFreq", 9000 },
            { "airGain", -3 }, { "satOn", 1 }, { "satDrive", 12 }, { "satType", 0 }, { "satMix", 60 }, { "dlyOn", 1 },
            { "dlySync", 4 }, { "dlyFb", 25 }, { "dlyTone", 2500 }, { "dlyMix", 8 }, { "dlyPing", 0 }, { "revOn", 1 },
            { "revDecay", 0.8f }, { "revPre", 8 }, { "revDamp", 2500 }, { "revMix", 8 }, { "outGain", 1.5f } } },

        { "Artist: Newlightchild style (cold, glassy)", {
            { "hpfFreq", 110 }, { "gateOn", 1 }, { "gateThr", -48 }, { "gateRange", 20 }, { "tamOn", 1 },
            { "tamFreq", 320 }, { "tamThr", -29 }, { "tamAmt", 4 }, { "dsFreq", 8000 }, { "dsThr", -28 },
            { "dsAmt", 10 }, { "c1Thr", -22 }, { "c1Ratio", 4 }, { "c1Atk", 10 }, { "c1Mkup", 4 }, { "c2On", 1 },
            { "c2Thr", -14 }, { "c2Ratio", 5 }, { "c2Atk", 2 }, { "c2Mkup", 2 }, { "lmFreq", 350 }, { "lmGain", -4 },
            { "prFreq", 5000 }, { "prGain", 2.5f }, { "airFreq", 14000 }, { "airGain", 6 }, { "satOn", 1 },
            { "satDrive", 6 }, { "satType", 1 }, { "satMix", 30 }, { "excOn", 1 }, { "excFreq", 7000 },
            { "excAmt", 40 }, { "dblOn", 1 }, { "dblMix", 45 }, { "dblWidth", 200 }, { "dblTime", 18 }, { "dlyOn", 1 },
            { "dlySync", 3 }, { "dlyFb", 40 }, { "dlyTone", 6000 }, { "dlyMix", 20 }, { "revOn", 1 },
            { "revDecay", 3 }, { "revPre", 35 }, { "revDamp", 9000 }, { "revLow", 300 }, { "revMix", 27 } } },

        { "Artist: LOVV66 style (glossy, close)", {
            { "hpfFreq", 100 }, { "dsThr", -30 }, { "dsAmt", 11 }, { "c1Thr", -25 }, { "c1Ratio", 5 }, { "c1Atk", 15 },
            { "c1Mkup", 5 }, { "c2On", 1 }, { "c2Thr", -17 }, { "c2Ratio", 6 }, { "c2Atk", 3 }, { "c2Rel", 60 },
            { "c2Mkup", 2 }, { "lsGain", 0.5f }, { "lmGain", -2.5f }, { "prFreq", 4200 }, { "prGain", 4.5f },
            { "airFreq", 11000 }, { "airGain", 4 }, { "satOn", 1 }, { "satDrive", 9 }, { "satType", 0 },
            { "satMix", 35 }, { "excOn", 1 }, { "excFreq", 6000 }, { "excAmt", 20 }, { "dblOn", 1 }, { "dblMix", 14 },
            { "dblWidth", 100 }, { "dblTime", 14 }, { "dlyOn", 1 }, { "dlySync", 4 }, { "dlyFb", 20 },
            { "dlyTone", 6000 }, { "dlyMix", 9 }, { "revOn", 1 }, { "revDecay", 1.3f }, { "revPre", 15 },
            { "revDamp", 9000 }, { "revMix", 13 }, { "outGain", 2 } } },

        { "Artist: Egor Krid style (clean pop, polished)", {
            { "hpfFreq", 100 }, { "dsFreq", 7000 }, { "dsThr", -30 }, { "dsAmt", 11 }, { "c1Thr", -19 },
            { "c1Atk", 22 }, { "c1Mkup", 3 }, { "c2On", 1 }, { "c2Thr", -11 }, { "c2Ratio", 3 }, { "c2Atk", 5 },
            { "c2Rel", 80 }, { "c2Mkup", 1.5f }, { "lsFreq", 110 }, { "lsGain", 1.5f }, { "lmFreq", 280 },
            { "lmGain", -2 }, { "prFreq", 4000 }, { "prGain", 3 }, { "airFreq", 12000 }, { "airGain", 5.5f },
            { "excOn", 1 }, { "excFreq", 6500 }, { "excAmt", 35 }, { "dblOn", 1 }, { "dblMix", 18 },
            { "dblWidth", 120 }, { "dblTime", 15 }, { "dlyOn", 1 }, { "dlySync", 4 }, { "dlyFb", 22 },
            { "dlyTone", 7000 }, { "dlyMix", 9 }, { "revOn", 1 }, { "revDecay", 1.7f }, { "revPre", 20 },
            { "revDamp", 10000 }, { "revMix", 13 }, { "outGain", -1.5f } } },

        { "Artist: Pepel Nahudi style (lo-fi tape, mid-honk)", {
            { "hpfFreq", 150 }, { "gateOn", 1 }, { "gateThr", -48 }, { "gateRange", 22 }, { "tamOn", 1 },
            { "tamThr", -29 }, { "tamAmt", 4 }, { "c1Thr", -24 }, { "c1Ratio", 5 }, { "c1Atk", 8 }, { "c1Mkup", 5 },
            { "c2On", 1 }, { "c2Thr", -14 }, { "c2Ratio", 6 }, { "c2Atk", 2 }, { "c2Mkup", 2 }, { "lsGain", 1 },
            { "lmGain", -2.5f }, { "prFreq", 1800 }, { "prGain", 4 }, { "airFreq", 7000 }, { "airGain", -5 },
            { "satOn", 1 }, { "satDrive", 14 }, { "satType", 1 }, { "satMix", 60 }, { "dlyOn", 1 }, { "dlySync", 5 },
            { "dlyFb", 28 }, { "dlyTone", 2500 }, { "dlyMix", 12 }, { "dlyPing", 0 }, { "revOn", 1 },
            { "revDecay", 0.9f }, { "revPre", 10 }, { "revDamp", 3500 }, { "revMix", 9 }, { "outGain", 1.5f } } },

        { "Artist: Platina style (tight, punchy, bright)", {
            { "hpfFreq", 110 }, { "dsThr", -27 }, { "dsAmt", 10 }, { "c1Thr", -26 }, { "c1Ratio", 7 }, { "c1Atk", 6 },
            { "c1Mkup", 6 }, { "c2On", 1 }, { "c2Thr", -18 }, { "c2Ratio", 10 }, { "c2Atk", 0.8f }, { "c2Rel", 45 },
            { "c2Mkup", 2 }, { "lsGain", 3 }, { "lmFreq", 330 }, { "lmGain", -2.5f }, { "prFreq", 4000 },
            { "prGain", 3.5f }, { "airFreq", 12000 }, { "airGain", 5 }, { "satOn", 1 }, { "satDrive", 16 },
            { "satType", 0 }, { "satMix", 55 }, { "excOn", 1 }, { "excFreq", 5500 }, { "excAmt", 50 }, { "dblOn", 1 },
            { "dblMix", 40 }, { "dblWidth", 170 }, { "dblTime", 12 }, { "dlyOn", 1 }, { "dlySync", 5 },
            { "dlyFb", 34 }, { "dlyTone", 6000 }, { "dlyMix", 16 }, { "revOn", 1 }, { "revDecay", 1 },
            { "revPre", 12 }, { "revMix", 8 }, { "outGain", 4 } } },

        { "Artist: OGbuda style (warm, thick, doubled)", {
            { "hpfFreq", 90 }, { "dsThr", -30 }, { "dsAmt", 9 }, { "c1Thr", -21 }, { "c1Atk", 18 }, { "c1Mkup", 3 },
            { "lsFreq", 130 }, { "lsGain", 3 }, { "lmFreq", 250 }, { "lmGain", 1.5f }, { "prFreq", 3000 },
            { "prGain", 1 }, { "airGain", 0 }, { "satOn", 1 }, { "satDrive", 8 }, { "satType", 0 }, { "satMix", 50 },
            { "dblOn", 1 }, { "dblMix", 55 }, { "dblWidth", 110 }, { "dblTime", 24 }, { "dlyOn", 1 }, { "dlySync", 3 },
            { "dlyFb", 36 }, { "dlyTone", 3500 }, { "dlyMix", 22 }, { "revOn", 1 }, { "revDecay", 1.3f },
            { "revPre", 20 }, { "revDamp", 5000 }, { "revMix", 11 } } },

        { "Artist: Toxi$ style (gritty, glitchy repeats)", {
            { "hpfFreq", 140 }, { "gateOn", 1 }, { "gateThr", -48 }, { "gateRange", 22 }, { "dsThr", -26 },
            { "dsAmt", 11 }, { "c1Thr", -25 }, { "c1Ratio", 6 }, { "c1Atk", 5 }, { "c1Mkup", 5 }, { "c2On", 1 },
            { "c2Thr", -15 }, { "c2Ratio", 8 }, { "c2Atk", 1 }, { "c2Rel", 45 }, { "c2Mkup", 3 }, { "lmFreq", 400 },
            { "lmGain", -3 }, { "prFreq", 3000 }, { "prGain", 3.5f }, { "airFreq", 11000 }, { "airGain", 0 },
            { "satOn", 1 }, { "satDrive", 21 }, { "satType", 2 }, { "satMix", 45 }, { "excOn", 1 },
            { "excFreq", 4500 }, { "excAmt", 40 }, { "dblOn", 1 }, { "dblMix", 30 }, { "dblWidth", 170 },
            { "dblTime", 11 }, { "dlyOn", 1 }, { "dlySync", 5 }, { "dlyFb", 55 }, { "dlyTone", 3500 },
            { "dlyMix", 30 }, { "revOn", 1 }, { "revDecay", 0.9f }, { "revPre", 6 }, { "revMix", 7 },
            { "lsGain", 2.5f }, { "outGain", 1 } } },

        { "Artist: Sqwoz Bab style (nasal, forward, dry)", {
            { "hpfFreq", 140 }, { "gateOn", 1 }, { "gateThr", -50 }, { "gateRange", 18 }, { "dsThr", -28 },
            { "dsAmt", 9 }, { "c1Thr", -22 }, { "c1Ratio", 5 }, { "c1Atk", 8 }, { "c1Mkup", 4 }, { "lsGain", -1 },
            { "lmFreq", 400 }, { "lmGain", -1 }, { "prFreq", 2500 }, { "prGain", 6 }, { "prQ", 1.4f },
            { "airGain", 4 }, { "satOn", 1 }, { "satDrive", 11 }, { "satType", 0 }, { "satMix", 50 }, { "excOn", 1 },
            { "excAmt", 25 }, { "dblOn", 1 }, { "dblMix", 22 }, { "dblWidth", 80 }, { "dblTime", 12 }, { "dlyOn", 1 },
            { "dlySync", 5 }, { "dlyFb", 18 }, { "dlyMix", 8 }, { "revOn", 1 }, { "revDecay", 0.8f }, { "revPre", 8 },
            { "revMix", 6 }, { "c2On", 1 }, { "c2Thr", -14 }, { "c2Ratio", 6 }, { "c2Atk", 2 }, { "c2Mkup", 2 } } },

        // ================= Genre / experimental / master =================
        { "Genre: Dark Trap", {
            { "hpfFreq", 90 }, { "tamOn", 1 }, { "tamThr", -29 }, { "tamAmt", 5 },
            { "c1Thr", -22 }, { "c1Ratio", 4 }, { "c1Mkup", 4 }, { "c2On", 1 }, { "c2Thr", -13 }, { "c2Ratio", 5 }, { "c2Atk", 2 },
            { "lsGain", 2.0f }, { "lmGain", -1.5f }, { "airGain", -1.0f },
            { "satOn", 1 }, { "satDrive", 9 }, { "satType", 0 }, { "satMix", 45 },
            { "dblOn", 1 }, { "dblMix", 20 },
            { "dlyOn", 1 }, { "dlySync", 2 }, { "dlyFb", 40 }, { "dlyTone", 2800 }, { "dlyMix", 15 },
            { "revOn", 1 }, { "revDecay", 3.0f }, { "revPre", 45 }, { "revDamp", 4500 }, { "revMix", 22 } } },

        { "Genre: Lyrical Rap (dry, present)", {
            { "hpfFreq", 90 }, { "gateOn", 1 }, { "gateThr", -52 }, { "gateRange", 16 },
            { "dsThr", -29 }, { "dsAmt", 8 },
            { "c1Thr", -20 }, { "c1Ratio", 3 }, { "c1Atk", 18 }, { "c1Mkup", 3 },
            { "lmGain", -1.5f }, { "prGain", 2.0f }, { "airGain", 1.5f },
            { "satOn", 1 }, { "satDrive", 3 }, { "satType", 0 }, { "satMix", 20 },
            { "excOn", 1 }, { "excAmt", 10 },
            { "dblOn", 1 }, { "dblMix", 8 },
            { "revOn", 1 }, { "revDecay", 1.0f }, { "revPre", 15 }, { "revMix", 7 } } },

        { "Experimental: Broken Radio", {
            { "hpfFreq", 350 }, { "hpfSlope", 0 }, { "dsOn", 0 },
            { "c1Thr", -26 }, { "c1Ratio", 8 }, { "c1Mkup", 8 },
            { "lmFreq", 700 }, { "lmGain", 3.0f }, { "prFreq", 1800 }, { "prGain", 5.0f }, { "airFreq", 6000 }, { "airGain", -12.0f },
            { "satOn", 1 }, { "satDrive", 16 }, { "satType", 2 }, { "satMix", 80 },
            { "dblOn", 1 }, { "dblMix", 40 }, { "dblWidth", 200 }, { "dblTime", 25 },
            { "dlyOn", 1 }, { "dlySync", 5 }, { "dlyFb", 55 }, { "dlyTone", 2000 }, { "dlyMix", 25 },
            { "revOn", 1 }, { "revDecay", 1.5f }, { "revDamp", 2500 }, { "revMix", 15 } } },

        { "Experimental: Space Choir", {
            { "hpfFreq", 150 }, { "c1Thr", -20 },
            { "airFreq", 12000 }, { "airGain", 5.0f },
            { "satOn", 1 }, { "satDrive", 4 }, { "satType", 1 }, { "satMix", 25 },
            { "excOn", 1 }, { "excAmt", 40 },
            { "dblOn", 1 }, { "dblMix", 80 }, { "dblWidth", 200 }, { "dblTime", 28 },
            { "dlyOn", 1 }, { "dlySync", 1 }, { "dlyFb", 60 }, { "dlyTone", 3500 }, { "dlyMix", 35 },
            { "revOn", 1 }, { "revDecay", 7.0f }, { "revPre", 80 }, { "revDamp", 7000 }, { "revMix", 45 } } },

        { "Experimental: Underwater", {
            { "hpfFreq", 70 }, { "lsGain", 3.0f }, { "lmFreq", 250 }, { "lmGain", 2.0f }, { "airFreq", 6000 }, { "airGain", -12.0f },
            { "satOn", 1 }, { "satDrive", 6 }, { "satType", 1 }, { "satMix", 40 },
            { "dblOn", 1 }, { "dblMix", 60 }, { "dblWidth", 150 }, { "dblTime", 30 },
            { "dlyOn", 1 }, { "dlySync", 3 }, { "dlyFb", 55 }, { "dlyTone", 1200 }, { "dlyMix", 25 },
            { "revOn", 1 }, { "revDecay", 5.0f }, { "revDamp", 1800 }, { "revMix", 35 } } },

        { "Beat: Master Glue & Polish", {
            { "hpfFreq", 25 }, { "hpfSlope", 0 }, { "dsOn", 0 },
            { "c1Thr", -14 }, { "c1Ratio", 2 }, { "c1Atk", 30 }, { "c1Rel", 200 }, { "c1Mkup", 1.5f },
            { "lsFreq", 60 }, { "lsGain", 1.0f }, { "lmGain", -1.0f }, { "airFreq", 12000 }, { "airGain", 1.5f },
            { "satOn", 1 }, { "satDrive", 3 }, { "satType", 1 }, { "satMix", 25 },
            { "limCeil", -1.0f }, { "limRel", 100 } } },

        // ================= Basic starting points =================
        { "Basic: Clean Pop", {
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

        { "Basic: Rap Upfront", {
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

        { "Basic: Airy Female", {
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

        { "Basic: Warm Male", {
            { "hpfFreq", 70 }, { "gateOn", 1 }, { "gateThr", -50 }, { "gateRange", 20 },
            { "tamOn", 1 }, { "tamFreq", 250 }, { "tamThr", -28 }, { "tamAmt", 5 },
            { "dsOn", 1 }, { "dsFreq", 6000 },
            { "c1On", 1 }, { "c1Thr", -20 }, { "c1Ratio", 3.5f }, { "c1Mkup", 3 },
            { "lsFreq", 120 }, { "lsGain", 2.0f }, { "lmFreq", 280 }, { "lmGain", -1.5f }, { "prFreq", 3000 }, { "prGain", 1.5f }, { "airGain", 2.0f },
            { "satOn", 1 }, { "satDrive", 8 }, { "satType", 0 }, { "satMix", 45 },
            { "excOn", 1 }, { "excAmt", 15 },
            { "revOn", 1 }, { "revDecay", 1.4f }, { "revMix", 10 } } },

        { "Basic: Podcast / Voiceover", {
            { "hpfFreq", 90 }, { "gateOn", 1 }, { "gateThr", -48 }, { "gateRange", 24 },
            { "hissOn", 1 }, { "ridOn", 1 }, { "ridTarget", -20 }, { "ridRange", 6 }, { "ridSpeed", 700 },
            { "dsOn", 1 }, { "dsThr", -30 }, { "dsAmt", 6 },
            { "c1On", 1 }, { "c1Thr", -22 }, { "c1Ratio", 3 }, { "c1Mkup", 4 },
            { "c2On", 1 }, { "c2Thr", -14 }, { "c2Ratio", 3 },
            { "lmFreq", 250 }, { "lmGain", -2.0f }, { "prFreq", 3000 }, { "prGain", 2.5f }, { "airGain", 1.0f } } },

        { "Basic: Lo-Fi Radio", {
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
