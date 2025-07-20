#include "PluginProcessor.h"
#include "PluginEditor.h"

AnalogDelayAudioProcessorEditor::AnalogDelayAudioProcessorEditor (AnalogDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), parameters(vts)
{
    setSize (420, 260);

    // Delay Time
    delayTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    delayTimeSlider.setRange(1.0, 2000.0, 0.1);
    delayTimeSlider.setSkewFactorFromMidPoint(400.0);
    delayTimeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    delayTimeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "delayTime", delayTimeSlider));
    addAndMakeVisible(delayTimeSlider);
    delayTimeLabel.setText("Delay Time (ms)", juce::dontSendNotification);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayTimeLabel);

    // Sync
    syncButton.setButtonText("Sync to Tempo");
    syncButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::aqua);
    syncAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(parameters, "sync", syncButton));
    addAndMakeVisible(syncButton);

    // Sync Division
    syncDivisionBox.addItemList({ "1/1", "1/2", "1/4", "1/8", "1/16", "1/8T", "1/16T" }, 1);
    syncDivisionAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(parameters, "syncDivision", syncDivisionBox));
    addAndMakeVisible(syncDivisionBox);
    syncDivisionLabel.setText("Sync Division", juce::dontSendNotification);
    syncDivisionLabel.attachToComponent(&syncDivisionBox, false);
    syncDivisionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(syncDivisionLabel);

    // Feedback
    feedbackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    feedbackSlider.setRange(0.0, 0.95, 0.001);
    feedbackSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightgreen);
    feedbackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "feedback", feedbackSlider));
    addAndMakeVisible(feedbackSlider);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(feedbackLabel);

    // Mix
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    mixSlider.setRange(0.0, 1.0, 0.001);
    mixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::deepskyblue);
    mixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "mix", mixSlider));
    addAndMakeVisible(mixSlider);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);
}

AnalogDelayAudioProcessorEditor::~AnalogDelayAudioProcessorEditor() {}

void AnalogDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(0xff181c20));
    g.setColour (juce::Colours::white);
    g.setFont (22.0f);
    g.drawFittedText ("Analog Delay", 0, 10, getWidth(), 28, juce::Justification::centred, 1);

    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(10, 45, getWidth()-20, getHeight()-55, 12.0f, 2.0f);
}

void AnalogDelayAudioProcessorEditor::resized()
{
    const int margin = 18;
    const int knobW = 90;
    const int knobH = 110;
    const int rowY = 60;
    const int row2Y = 170;

    delayTimeSlider.setBounds(margin, rowY, knobW, knobH);
    feedbackSlider.setBounds(margin + knobW + 18, rowY, knobW, knobH);
    mixSlider.setBounds(margin + 2*(knobW + 18), rowY, knobW, knobH);

    syncButton.setBounds(margin, row2Y, 120, 24);
    syncDivisionBox.setBounds(margin + 140, row2Y, 100, 24);
}
```

```cmake
#