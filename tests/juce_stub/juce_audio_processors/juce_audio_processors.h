// Minimal fake of the JUCE API surface used by VocalForge -- ONLY for catching typos / macro errors with g++ -fsyntax-only.
#pragma once
#include <algorithm>
#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#define JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(x) x (const x&) = delete;
#define JUCE_CALLTYPE
namespace juce {
template <class T> T jmin (T a, T b) { return a < b ? a : b; }
template <class T> T jmax (T a, T b) { return a > b ? a : b; }
template <class T> T jlimit (T lo, T hi, T v) { return v < lo ? lo : (v > hi ? hi : v); }
enum NotificationType { dontSendNotification, sendNotification };
struct String { std::string s; String() {} String (const char* c) : s (c) {} String (float, int) {}
    bool isEmpty() const { return s.empty(); } bool isNotEmpty() const { return ! s.empty(); }
    String operator+ (const String& o) const { return String ((s + o.s).c_str()); } };
using StringRef = String; using Identifier = String;
struct StringArray { static StringArray fromTokens (const String&, const String&, const String&) { return {}; } };
template <class T> struct NormalisableRange { NormalisableRange (T, T) {} void setSkewForCentre (T) {} T convertTo0to1 (T v) const { return v; } };
struct ParameterID { template <class S> ParameterID (S&&, int = 0) {} };
struct AudioParameterFloatAttributes { AudioParameterFloatAttributes withStringFromValueFunction (std::function<String (float, int)>) const { return *this; } };
struct AudioProcessorParameter { virtual ~AudioProcessorParameter() = default; float getDefaultValue() const { return 0; } void setValueNotifyingHost (float) {} };
struct RangedAudioParameter : AudioProcessorParameter { float convertTo0to1 (float v) const { return v; } };
struct AudioParameterFloat : RangedAudioParameter { AudioParameterFloat (const ParameterID&, const String&, NormalisableRange<float>, float, const AudioParameterFloatAttributes& = {}) {} };
struct AudioParameterBool : RangedAudioParameter { AudioParameterBool (const ParameterID&, const String&, bool) {} };
struct AudioParameterChoice : RangedAudioParameter { AudioParameterChoice (const ParameterID&, const String&, StringArray, int) {} StringArray choices; };
struct XmlElement { bool hasTagName (const String&) const { return true; } };
struct ValueTree { String getType() const { return {}; } std::unique_ptr<XmlElement> createXml() const { return nullptr; } static ValueTree fromXml (const XmlElement&) { return {}; } };
struct MemoryBlock {}; struct MidiBuffer {};
template <class T> struct Optional { bool hasValue() const { return false; } T& operator*() { return v; } T* operator->() { return &v; } T v{}; };
struct PositionInfo { Optional<double> getBpm() const { return {}; } };
struct AudioPlayHead { Optional<PositionInfo> getPosition() const { return {}; } };
template <class T> struct AudioBuffer { int getNumSamples() const { return 0; } int getNumChannels() const { return 2; } T* getWritePointer (int) { return nullptr; } };
struct ScopedNoDenormals {};
struct AudioChannelSet { static AudioChannelSet stereo() { return {}; } bool operator== (const AudioChannelSet&) const { return true; } };
class AudioProcessorEditor;
class AudioProcessor {
public:
    struct BusesProperties { BusesProperties withInput (const String&, AudioChannelSet, bool) const { return *this; } BusesProperties withOutput (const String&, AudioChannelSet, bool) const { return *this; } };
    struct BusesLayout { AudioChannelSet getMainInputChannelSet() const { return {}; } AudioChannelSet getMainOutputChannelSet() const { return {}; } };
    AudioProcessor (const BusesProperties&) {}
    virtual ~AudioProcessor() = default;
    virtual void prepareToPlay (double, int) = 0; virtual void releaseResources() = 0;
    virtual bool isBusesLayoutSupported (const BusesLayout&) const { return true; }
    virtual void processBlock (AudioBuffer<float>&, MidiBuffer&) = 0;
    virtual AudioProcessorEditor* createEditor() = 0; virtual bool hasEditor() const = 0;
    virtual const String getName() const = 0; virtual bool acceptsMidi() const = 0; virtual bool producesMidi() const = 0;
    virtual bool isMidiEffect() const { return false; } virtual double getTailLengthSeconds() const = 0;
    virtual int getNumPrograms() = 0; virtual int getCurrentProgram() = 0; virtual void setCurrentProgram (int) = 0;
    virtual const String getProgramName (int) = 0; virtual void changeProgramName (int, const String&) = 0;
    virtual void getStateInformation (MemoryBlock&) = 0; virtual void setStateInformation (const void*, int) = 0;
    const std::vector<AudioProcessorParameter*>& getParameters() const { static std::vector<AudioProcessorParameter*> v; return v; }
    AudioPlayHead* getPlayHead() const { return nullptr; }
    void setLatencySamples (int) {}
    static void copyXmlToBinary (const XmlElement&, MemoryBlock&) {}
    static std::unique_ptr<XmlElement> getXmlFromBinary (const void*, int) { return nullptr; }
};
class AudioProcessorValueTreeState {
public:
    struct ParameterLayout { template <class T> void add (std::unique_ptr<T>) {} };
    AudioProcessorValueTreeState (AudioProcessor&, void*, const String&, ParameterLayout) {}
    std::atomic<float>* getRawParameterValue (const StringRef&) const { static std::atomic<float> a; return &a; }
    RangedAudioParameter* getParameter (const StringRef&) const { static AudioParameterChoice p (ParameterID ("x"), "x", {}, 0); return &p; }
    ValueTree state; ValueTree copyState() { return {}; } void replaceState (const ValueTree&) {}
    struct SliderAttachment { template <class S> SliderAttachment (AudioProcessorValueTreeState&, const String&, S&) {} };
    struct ButtonAttachment { template <class S> ButtonAttachment (AudioProcessorValueTreeState&, const String&, S&) {} };
    struct ComboBoxAttachment { template <class S> ComboBoxAttachment (AudioProcessorValueTreeState&, const String&, S&) {} };
};
// ---------------- GUI ----------------
struct Colour { Colour() {} Colour (unsigned) {} Colour darker (float) const { return *this; } Colour brighter (float) const { return *this; } };
struct Colours { static inline Colour transparentBlack, black; };
template <class T> struct Point { T x, y; };
template <class T> struct Rectangle {
    T x{}, y{}, w{}, h{}; Rectangle() {} Rectangle (T a, T b, T c, T d) : x (a), y (b), w (c), h (d) {}
    T getWidth() const { return w; } T getHeight() const { return h; } Point<T> getCentre() const { return { x + w / 2, y + h / 2 }; }
    Rectangle reduced (T a) const { return *this; } Rectangle reduced (T a, T b) const { return *this; }
    Rectangle removeFromTop (T) { return *this; } Rectangle removeFromBottom (T) { return *this; }
    Rectangle removeFromLeft (T) { return *this; } Rectangle removeFromRight (T) { return *this; }
    Rectangle withSizeKeepingCentre (T, T) const { return *this; } Rectangle translated (T, T) const { return *this; }
    Rectangle withTrimmedLeft (T) const { return *this; } Rectangle withWidth (T) const { return *this; }
    Rectangle<float> toFloat() const { return {}; } };
struct Justification { enum Flags { centred, centredLeft }; Justification (Flags) {} };
struct AffineTransform { static AffineTransform rotation (float) { return {}; } AffineTransform translated (float, float) const { return *this; } };
struct PathStrokeType { enum JointStyle { curved }; enum EndCapStyle { rounded }; PathStrokeType (float, JointStyle, EndCapStyle) {} };
struct Path { void addCentredArc (float, float, float, float, float, float, float, bool) {} void addRoundedRectangle (float, float, float, float, float) {} void applyTransform (const AffineTransform&) {} };
struct Graphics { void setColour (Colour) {} void fillAll (Colour) {} void fillRoundedRectangle (Rectangle<float>, float) {} void drawRoundedRectangle (Rectangle<float>, float, float) {}
    void fillEllipse (float, float, float, float) {} void strokePath (const Path&, const PathStrokeType&) {} void fillPath (const Path&) {} void setFont (float) {}
    void drawText (const String&, int, int, int, int, Justification, bool) {} void drawText (const String&, Rectangle<int>, Justification, bool) {} };
class LookAndFeel {}; 
class Component { public: virtual ~Component() = default; virtual void paint (Graphics&) {} virtual void resized() {}
    void addAndMakeVisible (Component&) {} void addChildComponent (Component&) {} void setBounds (Rectangle<int>) {} void setBounds (int, int, int, int) {}
    Rectangle<int> getLocalBounds() const { return {}; } int getWidth() const { return 0; } void setVisible (bool) {} void repaint() {}
    void setInterceptsMouseClicks (bool, bool) {} void setLookAndFeel (LookAndFeel*) {} void setSize (int, int) {} };
class LookAndFeel_V4 : public LookAndFeel { public: void setColour (int, Colour) {}
    virtual void drawRotarySlider (Graphics&, int, int, int, int, float, float, float, struct Slider&) {}
    virtual void drawToggleButton (Graphics&, struct ToggleButton&, bool, bool) {} };
struct Slider : Component { enum SliderStyle { RotaryHorizontalVerticalDrag }; enum TextEntryBoxPosition { TextBoxBelow };
    enum ColourIds { textBoxTextColourId, textBoxOutlineColourId, textBoxBackgroundColourId };
    void setSliderStyle (SliderStyle) {} void setTextBoxStyle (TextEntryBoxPosition, bool, int, int) {} };
struct Label : Component { enum ColourIds { textColourId }; void setText (const String&, NotificationType) {} void setJustificationType (Justification) {} };
struct Button : Component { std::function<void()> onClick; void setButtonText (const String&) {} String getButtonText() const { return {}; } bool getToggleState() const { return false; }
    void setToggleState (bool, NotificationType) {} };
struct ToggleButton : Button { ToggleButton() {} ToggleButton (const String&) {} };
struct TextButton : Button { enum ColourIds { buttonColourId, buttonOnColourId, textColourOffId, textColourOnId }; void setClickingTogglesState (bool) {} void setRadioGroupId (int) {} };
struct ComboBox : Component { enum ColourIds { backgroundColourId, textColourId, outlineColourId, arrowColourId }; std::function<void()> onChange;
    void addItem (const String&, int) {} void addItemList (const StringArray&, int) {} void setTextWhenNothingSelected (const String&) {} int getSelectedId() const { return 0; } };
struct PopupMenu { enum ColourIds { backgroundColourId, textColourId, highlightedBackgroundColourId, highlightedTextColourId }; };
struct FlexItem { struct Margin { Margin (float) {} }; FlexItem (Component&) {} FlexItem withWidth (float) const { return *this; } FlexItem withHeight (float) const { return *this; } FlexItem withMargin (Margin) const { return *this; } };
struct FlexBox { enum class Wrap { wrap }; enum class JustifyContent { flexStart }; enum class AlignContent { flexStart };
    Wrap flexWrap; JustifyContent justifyContent; AlignContent alignContent; struct Items { void add (const FlexItem&) {} } items; void performLayout (Rectangle<float>) {} };
struct Timer { virtual ~Timer() = default; virtual void timerCallback() = 0; void startTimerHz (int) {} };
class AudioProcessorEditor : public Component { public: AudioProcessorEditor (AudioProcessor*) {} };
} // namespace juce
