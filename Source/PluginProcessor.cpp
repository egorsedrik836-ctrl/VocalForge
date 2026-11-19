#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets.h"

namespace
{
juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto addFloat = [&layout] (const char* id, const char* name, float mn, float mx, float def,
                               float centre, const char* unit, int decimals)
    {
        juce::NormalisableRange<float> range (mn, mx);
        if (centre > mn && centre < mx)
            range.setSkewForCentre (centre);

        const juce::String u (unit);
        const juce::String suffix = u.isEmpty() ? juce::String() : juce::String (" ") + u;

        auto attrs = juce::AudioParameterFloatAttributes().withStringFromValueFunction (
            [suffix, decimals] (float v, int) { return juce::String (v, decimals) + suffix; });

        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, def, attrs));
    };

#define VF_L_F(id, name, mn, mx, def, ctr, unit, dec) addFloat (#id, name, mn, mx, def, ctr, unit, dec);
#define VF_L_B(id, name, def) layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { #id, 1 }, name, (def) != 0));
#define VF_L_C(id, name, def, items) layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { #id, 1 }, name, juce::StringArray::fromTokens (items, "|", ""), def));
    VF_PARAM_LIST (VF_L_F, VF_L_B, VF_L_C)
#undef VF_L_F
#undef VF_L_B
#undef VF_L_C

    return layout;
}
} // namespace

VocalForgeAudioProcessor::VocalForgeAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "VocalForgeState", createLayout())
{
    for (auto& m : meters)
        m.store (0.0f);
    meters[0].store (-100.0f);
    meters[1].store (-100.0f);

#define VF_A_F(id, name, mn, mx, def, ctr, unit, dec) raw.id = apvts.getRawParameterValue (#id);
#define VF_A_B(id, name, def) raw.id = apvts.getRawParameterValue (#id);
#define VF_A_C(id, name, def, items) raw.id = apvts.getRawParameterValue (#id);
    VF_PARAM_LIST (VF_A_F, VF_A_B, VF_A_C)
#undef VF_A_F
#undef VF_A_B
#undef VF_A_C
}

void VocalForgeAudioProcessor::prepareToPlay (double sampleRate, int)
{
    chain.prepare (sampleRate);
    setLatencySamples (chain.getLatencySamples());
}

bool VocalForgeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void VocalForgeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int n = buffer.getNumSamples();
    if (buffer.getNumChannels() < 2 || n <= 0)
        return;

#define VF_F_F(id, name, mn, mx, def, ctr, unit, dec) params.id = raw.id->load();
#define VF_F_B(id, name, def) params.id = raw.id->load();
#define VF_F_C(id, name, def, items) params.id = raw.id->load();
    VF_PARAM_LIST (VF_F_F, VF_F_B, VF_F_C)
#undef VF_F_F
#undef VF_F_B
#undef VF_F_C

    // tempo-synced delay time
    const int sync = (int) (params.dlySync + 0.5f);
    if (sync > 0)
    {
        double bpm = 120.0;
        if (auto* ph = getPlayHead())
        {
            auto pos = ph->getPosition();
            if (pos.hasValue())
            {
                auto b = pos->getBpm();
                if (b.hasValue() && *b > 20.0)
                    bpm = *b;
            }
        }
        static const double beats[] = { 0.0, 2.0, 1.0, 0.75, 0.5, 0.25 };
        params.dlyTime = (float) juce::jmin (1990.0, 60000.0 / bpm * beats[juce::jlimit (0, 5, sync)]);
    }

    chain.process (buffer.getWritePointer (0), buffer.getWritePointer (1), n, params);

    meters[0].store (chain.meters.inDb);
    meters[1].store (chain.meters.outDb);
    meters[2].store (chain.meters.gate);
    meters[3].store (chain.meters.comp1);
    meters[4].store (chain.meters.deess);
    meters[5].store (chain.meters.comp2);
    meters[6].store (chain.meters.limiter);
}

void VocalForgeAudioProcessor::applyPreset (int index)
{
    const auto& presets = vf::getPresets();
    if (index < 0 || index >= (int) presets.size())
        return;

    for (auto* prm : getParameters())
        prm->setValueNotifyingHost (prm->getDefaultValue());

    for (const auto& v : presets[(size_t) index].values)
        if (auto* rp = apvts.getParameter (v.id))
            rp->setValueNotifyingHost (rp->convertTo0to1 (v.value));
}

juce::AudioProcessorEditor* VocalForgeAudioProcessor::createEditor()
{
    return new VocalForgeAudioProcessorEditor (*this);
}

void VocalForgeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void VocalForgeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalForgeAudioProcessor();
}
