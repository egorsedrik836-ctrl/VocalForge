#include "PluginEditor.h"
#include "Presets.h"

VocalForgeAudioProcessorEditor::VocalForgeAudioProcessorEditor (VocalForgeAudioProcessor& p)
    : AudioProcessorEditor (&p),
      proc (p),
      master (p.apvts, "MASTER", "", 6),
      meterStrip (p)
{
    setLookAndFeel (&laf);

    // ---- presets ----
    const auto& presets = vf::getPresets();
    for (size_t i = 0; i < presets.size(); ++i)
        presetBox.addItem (presets[i].name, (int) i + 1);
    presetBox.setTextWhenNothingSelected ("Load preset...");
    presetBox.onChange = [this] { proc.applyPreset (presetBox.getSelectedId() - 1); };
    addAndMakeVisible (presetBox);

    // ---- tabs ----
    static const char* tabNames[3] = { "CLEAN", "COLOR", "SPACE" };
    for (int i = 0; i < 3; ++i)
    {
        tabs[i].setButtonText (tabNames[i]);
        tabs[i].setClickingTogglesState (true);
        tabs[i].setRadioGroupId (1001);
        tabs[i].onClick = [this, i] { showPage (i); };
        addAndMakeVisible (tabs[i]);
        addChildComponent (pages[i]);
    }

    buildCleanPage();
    buildColorPage();
    buildSpacePage();

    master.knob ("inTrim", "Input Trim")
          .toggle ("monoIn", "MONO IN")
          .knob ("outGain", "Output Gain")
          .toggle ("limOn", "LIMITER")
          .knob ("limCeil", "Ceiling")
          .knob ("limRel", "Lim Release");
    addAndMakeVisible (master);
    addAndMakeVisible (meterStrip);

    setSize (1080, 700);
    showPage (0);
}

VocalForgeAudioProcessorEditor::~VocalForgeAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void VocalForgeAudioProcessorEditor::buildCleanPage()
{
    auto& s = proc.apvts;
    auto& pg = pages[0];

    pg.add (std::make_unique<Section> (s, "HIGH-PASS", "hpfOn", 2))
        .knob ("hpfFreq", "Freq").choice ("hpfSlope", "Slope");

    pg.add (std::make_unique<Section> (s, "NOISE GATE", "gateOn", 4))
        .knob ("gateThr", "Threshold").knob ("gateRange", "Range").knob ("gateAtk", "Attack").knob ("gateRel", "Release");

    pg.add (std::make_unique<Section> (s, "HISS REDUCER", "hissOn", 3))
        .knob ("hissFreq", "Above").knob ("hissThr", "Threshold").knob ("hissAmt", "Amount");

    pg.add (std::make_unique<Section> (s, "MUD / RESONANCE TAMER", "tamOn", 3))
        .knob ("tamFreq", "Freq").knob ("tamThr", "Threshold").knob ("tamAmt", "Amount");

    pg.add (std::make_unique<Section> (s, "VOCAL RIDER", "ridOn", 3))
        .knob ("ridTarget", "Target").knob ("ridRange", "Range").knob ("ridSpeed", "Speed");

    pg.add (std::make_unique<Section> (s, "DE-ESSER", "dsOn", 4))
        .knob ("dsFreq", "Freq").knob ("dsThr", "Threshold").knob ("dsAmt", "Amount").toggle ("dsListen", "LISTEN");
}

void VocalForgeAudioProcessorEditor::buildColorPage()
{
    auto& s = proc.apvts;
    auto& pg = pages[1];

    pg.add (std::make_unique<Section> (s, "TONE EQ", "eqOn", 5))
        .knob ("lsFreq", "Low Shelf").knob ("lmFreq", "Low-Mid").knob ("prFreq", "Presence").knob ("airFreq", "Air").knob ("lmQ", "LM Q")
        .knob ("lsGain", "LS Gain").knob ("lmGain", "LM Gain").knob ("prGain", "Pres Gain").knob ("airGain", "Air Gain").knob ("prQ", "Pres Q");

    pg.add (std::make_unique<Section> (s, "COMP 1 - LEVELER", "c1On", 5))
        .knob ("c1Thr", "Threshold").knob ("c1Ratio", "Ratio").knob ("c1Atk", "Attack").knob ("c1Rel", "Release").knob ("c1Mkup", "Makeup");

    pg.add (std::make_unique<Section> (s, "COMP 2 - PEAK", "c2On", 5))
        .knob ("c2Thr", "Threshold").knob ("c2Ratio", "Ratio").knob ("c2Atk", "Attack").knob ("c2Rel", "Release").knob ("c2Mkup", "Makeup");

    pg.add (std::make_unique<Section> (s, "SATURATION", "satOn", 3))
        .choice ("satType", "Type").knob ("satDrive", "Drive").knob ("satMix", "Mix");

    pg.add (std::make_unique<Section> (s, "EXCITER", "excOn", 2))
        .knob ("excFreq", "Above").knob ("excAmt", "Amount");
}

void VocalForgeAudioProcessorEditor::buildSpacePage()
{
    auto& s = proc.apvts;
    auto& pg = pages[2];

    pg.add (std::make_unique<Section> (s, "DOUBLER", "dblOn", 3))
        .knob ("dblMix", "Mix").knob ("dblWidth", "Width").knob ("dblTime", "Time");

    pg.add (std::make_unique<Section> (s, "DELAY", "dlyOn", 6))
        .choice ("dlySync", "Sync").knob ("dlyTime", "Time").knob ("dlyFb", "Feedback")
        .knob ("dlyTone", "Tone").knob ("dlyMix", "Mix").toggle ("dlyPing", "PING-PONG");

    pg.add (std::make_unique<Section> (s, "REVERB", "revOn", 5))
        .knob ("revDecay", "Decay").knob ("revPre", "Pre-Delay").knob ("revDamp", "Damping")
        .knob ("revLow", "Low Cut").knob ("revMix", "Mix");
}

void VocalForgeAudioProcessorEditor::showPage (int index)
{
    for (int i = 0; i < 3; ++i)
        pages[i].setVisible (i == index);
    tabs[index].setToggleState (true, juce::dontSendNotification);
}

void VocalForgeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (ui::bg());
    g.setColour (ui::text());
    g.setFont (24.0f);
    g.drawText ("VOCALFORGE", 22, 12, 260, 30, juce::Justification::centredLeft, false);
    g.setColour (ui::dim());
    g.setFont (13.0f);
    g.drawText ("full vocal chain: clean > color > space", 22, 38, 360, 18, juce::Justification::centredLeft, false);
}

void VocalForgeAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced (14);
    auto header = r.removeFromTop (48);
    presetBox.setBounds (header.removeFromRight (240).withSizeKeepingCentre (240, 28));

    auto tabRow = r.removeFromTop (36);
    for (int i = 0; i < 3; ++i)
        tabs[i].setBounds (tabRow.removeFromLeft (110).reduced (2));

    auto bottom = r.removeFromBottom (142);
    master.setBounds (bottom.removeFromLeft (master.preferredWidth()).reduced (0, 3));
    meterStrip.setBounds (bottom.withTrimmedLeft (10).reduced (0, 3));

    for (auto& p : pages)
        p.setBounds (r);
}
