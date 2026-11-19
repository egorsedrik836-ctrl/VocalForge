#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include "dsp/VocalChain.h"

class VocalForgeAudioProcessor : public juce::AudioProcessor
{
public:
    VocalForgeAudioProcessor();
    ~VocalForgeAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "VocalForge"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    // Presets are handled by the plug-in UI (the host sees a single program).
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void applyPreset (int index);

    juce::AudioProcessorValueTreeState apvts;

    // 0 in, 1 out (dBFS); 2 gate, 3 comp1, 4 de-esser, 5 comp2, 6 limiter (gain reduction, dB)
    std::atomic<float> meters[7];

private:
    struct Raw
    {
#define VF_R_F(id, name, mn, mx, def, ctr, unit, dec) std::atomic<float>* id = nullptr;
#define VF_R_B(id, name, def) std::atomic<float>* id = nullptr;
#define VF_R_C(id, name, def, items) std::atomic<float>* id = nullptr;
        VF_PARAM_LIST (VF_R_F, VF_R_B, VF_R_C)
#undef VF_R_F
#undef VF_R_B
#undef VF_R_C
    } raw;

    vf::VocalChain chain;
    vf::Params params;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalForgeAudioProcessor)
};
