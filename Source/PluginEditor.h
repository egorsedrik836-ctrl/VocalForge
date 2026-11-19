#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include <vector>
#include "PluginProcessor.h"

namespace ui
{
inline juce::Colour bg()        { return juce::Colour (0xff12151a); }
inline juce::Colour panel()     { return juce::Colour (0xff1b1f26); }
inline juce::Colour panelEdge() { return juce::Colour (0xff2c333d); }
inline juce::Colour accent()    { return juce::Colour (0xff35d0a8); }
inline juce::Colour accent2()   { return juce::Colour (0xfff2b84b); }
inline juce::Colour text()      { return juce::Colour (0xffe6e9ee); }
inline juce::Colour dim()       { return juce::Colour (0xff8b93a1); }
}

//==============================================================================
class VFLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VFLookAndFeel()
    {
        setColour (juce::Slider::textBoxTextColourId, ui::text());
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour (juce::Label::textColourId, ui::dim());
        setColour (juce::ComboBox::backgroundColourId, ui::panelEdge());
        setColour (juce::ComboBox::textColourId, ui::text());
        setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        setColour (juce::ComboBox::arrowColourId, ui::dim());
        setColour (juce::PopupMenu::backgroundColourId, ui::panel());
        setColour (juce::PopupMenu::textColourId, ui::text());
        setColour (juce::PopupMenu::highlightedBackgroundColourId, ui::accent().darker (0.5f));
        setColour (juce::PopupMenu::highlightedTextColourId, ui::text());
        setColour (juce::TextButton::buttonColourId, ui::panelEdge());
        setColour (juce::TextButton::buttonOnColourId, ui::accent());
        setColour (juce::TextButton::textColourOffId, ui::dim());
        setColour (juce::TextButton::textColourOnId, juce::Colours::black);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float pos,
                           float startAngle, float endAngle, juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (6.0f);
        const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto c = bounds.getCentre();
        const float arcR = radius - 3.0f;
        const float ang = startAngle + pos * (endAngle - startAngle);

        juce::Path track;
        track.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, startAngle, endAngle, true);
        g.setColour (ui::panelEdge());
        g.strokePath (track, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path value;
        value.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, startAngle, ang, true);
        g.setColour (ui::accent());
        g.strokePath (value, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour (ui::bg());
        g.fillEllipse (c.x - arcR + 7.0f, c.y - arcR + 7.0f, (arcR - 7.0f) * 2.0f, (arcR - 7.0f) * 2.0f);

        juce::Path pointer;
        pointer.addRoundedRectangle (-1.5f, -(arcR - 7.0f), 3.0f, (arcR - 7.0f) * 0.55f, 1.5f);
        pointer.applyTransform (juce::AffineTransform::rotation (ang).translated (c.x, c.y));
        g.setColour (ui::text());
        g.fillPath (pointer);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool) override
    {
        auto r = b.getLocalBounds().toFloat().reduced (1.0f);
        const bool on = b.getToggleState();
        g.setColour (on ? ui::accent() : ui::panelEdge());
        if (highlighted && ! on) g.setColour (ui::panelEdge().brighter (0.2f));
        g.fillRoundedRectangle (r, 5.0f);
        g.setColour (on ? juce::Colours::black : ui::dim());
        g.setFont (12.0f);
        g.drawText (b.getButtonText(), b.getLocalBounds(), juce::Justification::centred, false);
    }
};

//==============================================================================
using APVTS = juce::AudioProcessorValueTreeState;

class Knob : public juce::Component
{
public:
    Knob (APVTS& state, const juce::String& paramId, const juce::String& label)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 76, 16);
        addAndMakeVisible (slider);
        caption.setText (label, juce::dontSendNotification);
        caption.setJustificationType (juce::Justification::centred);
        caption.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (caption);
        attachment = std::make_unique<APVTS::SliderAttachment> (state, paramId, slider);
    }
    void resized() override
    {
        auto r = getLocalBounds();
        caption.setBounds (r.removeFromTop (16));
        slider.setBounds (r);
    }
private:
    juce::Slider slider;
    juce::Label caption;
    std::unique_ptr<APVTS::SliderAttachment> attachment;
};

class ToggleItem : public juce::Component
{
public:
    ToggleItem (APVTS& state, const juce::String& paramId, const juce::String& label)
    {
        button.setButtonText (label);
        addAndMakeVisible (button);
        attachment = std::make_unique<APVTS::ButtonAttachment> (state, paramId, button);
    }
    void resized() override { button.setBounds (getLocalBounds().withSizeKeepingCentre (getWidth() - 12, 26)); }
private:
    juce::ToggleButton button;
    std::unique_ptr<APVTS::ButtonAttachment> attachment;
};

class ChoiceItem : public juce::Component
{
public:
    ChoiceItem (APVTS& state, const juce::String& paramId, const juce::String& label)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (state.getParameter (paramId)))
            box.addItemList (p->choices, 1);
        addAndMakeVisible (box);
        caption.setText (label, juce::dontSendNotification);
        caption.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (caption);
        attachment = std::make_unique<APVTS::ComboBoxAttachment> (state, paramId, box);
    }
    void resized() override
    {
        auto r = getLocalBounds();
        caption.setBounds (r.removeFromTop (16));
        box.setBounds (r.withSizeKeepingCentre (getWidth() - 10, 26).translated (0, -8));
    }
private:
    juce::ComboBox box;
    juce::Label caption;
    std::unique_ptr<APVTS::ComboBoxAttachment> attachment;
};

//==============================================================================
class Section : public juce::Component
{
public:
    // onParam: ID of the on/off parameter shown in the header ("" for none)
    Section (APVTS& s, const juce::String& title, const juce::String& onParam, int columns)
        : state (s), titleText (title), cols (columns)
    {
        if (onParam.isNotEmpty())
        {
            powerBtn = std::make_unique<juce::ToggleButton> ("ON");
            addAndMakeVisible (*powerBtn);
            powerAtt = std::make_unique<APVTS::ButtonAttachment> (state, onParam, *powerBtn);
        }
    }
    Section& knob (const juce::String& id, const juce::String& label)   { return add (std::make_unique<Knob> (state, id, label)); }
    Section& choice (const juce::String& id, const juce::String& label) { return add (std::make_unique<ChoiceItem> (state, id, label)); }
    Section& toggle (const juce::String& id, const juce::String& label) { return add (std::make_unique<ToggleItem> (state, id, label)); }

    int preferredWidth() const  { return juce::jmax (150, 16 + juce::jmin (cols, (int) items.size()) * cellW); }
    int preferredHeight() const { return 34 + (((int) items.size() + cols - 1) / cols) * cellH + 6; }

    void paint (juce::Graphics& g) override
    {
        auto r = getLocalBounds().toFloat().reduced (1.0f);
        g.setColour (ui::panel());
        g.fillRoundedRectangle (r, 8.0f);
        g.setColour (ui::panelEdge());
        g.drawRoundedRectangle (r, 8.0f, 1.0f);
        g.setColour (ui::text());
        g.setFont (14.0f);
        g.drawText (titleText, 14, 8, getWidth() - 80, 20, juce::Justification::centredLeft, true);
    }
    void resized() override
    {
        if (powerBtn != nullptr)
            powerBtn->setBounds (getWidth() - 52, 9, 40, 20);
        int idx = 0;
        for (auto& it : items)
        {
            it->setBounds (8 + (idx % cols) * cellW, 34 + (idx / cols) * cellH, cellW, cellH);
            ++idx;
        }
    }
private:
    Section& add (std::unique_ptr<juce::Component> c)
    {
        addAndMakeVisible (*c);
        items.push_back (std::move (c));
        return *this;
    }
    static constexpr int cellW = 84, cellH = 96;
    APVTS& state;
    juce::String titleText;
    int cols;
    std::unique_ptr<juce::ToggleButton> powerBtn;
    std::unique_ptr<APVTS::ButtonAttachment> powerAtt;
    std::vector<std::unique_ptr<juce::Component>> items;
};

class Page : public juce::Component
{
public:
    Section& add (std::unique_ptr<Section> s)
    {
        addAndMakeVisible (*s);
        sections.push_back (std::move (s));
        return *sections.back();
    }
    void resized() override
    {
        juce::FlexBox fb;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        fb.alignContent = juce::FlexBox::AlignContent::flexStart;
        for (auto& s : sections)
            fb.items.add (juce::FlexItem (*s)
                              .withWidth ((float) s->preferredWidth())
                              .withHeight ((float) s->preferredHeight())
                              .withMargin (juce::FlexItem::Margin (5.0f)));
        fb.performLayout (getLocalBounds().toFloat());
    }
private:
    std::vector<std::unique_ptr<Section>> sections;
};

//==============================================================================
class MeterStrip : public juce::Component, private juce::Timer
{
public:
    explicit MeterStrip (VocalForgeAudioProcessor& p) : proc (p)
    {
        for (auto& v : shown) v = -100.0f;
        startTimerHz (30);
    }
    void paint (juce::Graphics& g) override
    {
        static const char* names[7] = { "IN", "OUT", "GATE", "COMP 1", "DE-ESS", "COMP 2", "LIMIT" };
        auto r = getLocalBounds().toFloat();
        g.setColour (ui::panel());
        g.fillRoundedRectangle (r.reduced (1.0f), 8.0f);
        g.setColour (ui::panelEdge());
        g.drawRoundedRectangle (r.reduced (1.0f), 8.0f, 1.0f);

        auto area = getLocalBounds().reduced (14, 10);
        const int rowH = area.getHeight() / 7;
        for (int i = 0; i < 7; ++i)
        {
            auto row = area.removeFromTop (rowH);
            g.setColour (ui::dim());
            g.setFont (12.0f);
            g.drawText (names[i], row.removeFromLeft (60), juce::Justification::centredLeft, false);
            auto bar = row.reduced (0, 3).toFloat();
            g.setColour (ui::panelEdge());
            g.fillRoundedRectangle (bar, 3.0f);

            const bool isGr = i >= 2;
            const float v = shown[i];
            const float frac = isGr ? juce::jlimit (0.0f, 1.0f, v / 24.0f)
                                    : juce::jlimit (0.0f, 1.0f, (v + 60.0f) / 60.0f);
            auto fill = bar.withWidth (bar.getWidth() * frac);
            g.setColour (isGr ? ui::accent2() : (v > -1.0f ? juce::Colour (0xffe5484d) : ui::accent()));
            g.fillRoundedRectangle (fill, 3.0f);
        }
    }
private:
    void timerCallback() override
    {
        for (int i = 0; i < 7; ++i)
        {
            const float raw = proc.meters[i].load();
            const float decay = i < 2 ? 1.6f : 0.9f;
            shown[i] = juce::jmax (raw, shown[i] - decay);
        }
        repaint();
    }
    VocalForgeAudioProcessor& proc;
    float shown[7];
};

//==============================================================================
class VocalForgeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit VocalForgeAudioProcessorEditor (VocalForgeAudioProcessor&);
    ~VocalForgeAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void buildCleanPage();
    void buildColorPage();
    void buildSpacePage();
    void showPage (int index);

    VocalForgeAudioProcessor& proc;
    VFLookAndFeel laf;

    juce::ComboBox presetBox;
    juce::TextButton tabs[3];
    Page pages[3];
    Section master;
    MeterStrip meterStrip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalForgeAudioProcessorEditor)
};
