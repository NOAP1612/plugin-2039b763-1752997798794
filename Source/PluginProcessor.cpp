#include "PluginProcessor.h"
#include "PluginEditor.h"

AnalogDelayAudioProcessor::AnalogDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                       ),
#endif
    parameters(*this, nullptr, juce::Identifier("AnalogDelayParams"),
    {
        std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time (ms)", juce::NormalisableRange<float>(1.0f, 2000.0f, 0.1f), 400.0f),
        std::make_unique<juce::AudioParameterBool>("sync", "Sync to Tempo", false),
        std::make_unique<juce::AudioParameterChoice>("syncDivision", "Sync Division",
            juce::StringArray({ "1/1", "1/2", "1/4", "1/8", "1/16", "1/8T", "1/16T" }), 2),
        std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", juce::NormalisableRange<float>(0.0f, 0.95f, 0.001f), 0.35f),
        std::make_unique<juce::AudioParameterFloat>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f)
    })
{
    delayTimeParam = parameters.getRawParameterValue("delayTime");
    syncParam = parameters.getRawParameterValue("sync");
    syncDivisionParam = parameters.getRawParameterValue("syncDivision");
    feedbackParam = parameters.getRawParameterValue("feedback");
    mixParam = parameters.getRawParameterValue("mix");
}

AnalogDelayAudioProcessor::~AnalogDelayAudioProcessor() {}

const juce::String AnalogDelayAudioProcessor::getName() const { return JucePlugin_Name; }

bool AnalogDelayAudioProcessor::acceptsMidi() const { return false; }
bool AnalogDelayAudioProcessor::producesMidi() const { return false; }
bool AnalogDelayAudioProcessor::isMidiEffect() const { return false; }
double AnalogDelayAudioProcessor::getTailLengthSeconds() const { return 2.0; }

int AnalogDelayAudioProcessor::getNumPrograms() { return 1; }
int AnalogDelayAudioProcessor::getCurrentProgram() { return 0; }
void AnalogDelayAudioProcessor::setCurrentProgram (int) {}
const juce::String AnalogDelayAudioProcessor::getProgramName (int) { return {}; }
void AnalogDelayAudioProcessor::changeProgramName (int, const juce::String&) {}

void AnalogDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    const int maxDelayMs = 2000;
    const int maxDelaySamples = (int)(maxDelayMs * 0.001 * sr) + 1;

    for (int ch = 0; ch < 2; ++ch)
    {
        delayBuffer[ch].setSize(1, maxDelaySamples);
        delayBuffer[ch].clear();
        writePosition[ch] = 0;
    }
}

void AnalogDelayAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AnalogDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    return true;
#endif
}
#endif

static float getSyncTimeMs(double bpm, int divisionIdx)
{
    // 1/1, 1/2, 1/4, 1/8, 1/16, 1/8T, 1/16T
    static const double multipliers[] = { 4.0, 2.0, 1.0, 0.5, 0.25, 1.0 / 3.0, 0.25 / 3.0 };
    double quarterNoteMs = 60000.0 / bpm;
    return (float)(quarterNoteMs * multipliers[divisionIdx]);
}

void AnalogDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const int numChannels = juce::jmin(2, buffer.getNumChannels());
    const int numSamples = buffer.getNumSamples();

    float delayTimeMs = *delayTimeParam;
    bool sync = (*syncParam) > 0.5f;
    int syncDiv = (int)(*syncDivisionParam);

    if (sync)
    {
        double bpm = 120.0;
        if (auto* playHead = getPlayHead())
        {
            juce::AudioPlayHead::CurrentPositionInfo info;
            if (playHead->getCurrentPosition(info) && info.bpm > 0.0)
                bpm = info.bpm;
        }
        delayTimeMs = getSyncTimeMs(bpm, syncDiv);
    }

    float feedback = *feedbackParam;
    float mix = *mixParam;

    int delaySamples = juce::jlimit(1, delayBuffer[0].getNumSamples() - 1, (int)(delayTimeMs * 0.001 * sr));

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        float* delayData = delayBuffer[ch].getWritePointer(0);
        int bufferLen = delayBuffer[ch].getNumSamples();

        int wp = writePosition[ch];

        for (int n = 0; n < numSamples; ++n)
        {
            int readPos = wp - delaySamples;
            if (readPos < 0)
                readPos += bufferLen;

            // Linear interpolation for analog-style delay
            int idx1 = readPos;
            int idx2 = (readPos + 1) % bufferLen;
            float frac = (delayTimeMs * 0.001f * sr) - delaySamples;
            float delayedSample = juce::jmap(frac, delayData[idx1], delayData[idx2]);

            // Simple analog-style lowpass on feedback path
            float fbSample = delayedSample * feedback;
            fbSample = lastFbSample[ch] = 0.7f * lastFbSample[ch] + 0.3f * fbSample;

            float inSample = channelData[n];
            float outSample = inSample * (1.0f - mix) + delayedSample * mix;

            delayData[wp] = inSample + fbSample;

            channelData[n] = outSample;

            wp = (wp + 1) % bufferLen;
        }
        writePosition[ch] = wp;
    }
}

bool AnalogDelayAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* AnalogDelayAudioProcessor::createEditor()
{
    return new AnalogDelayAudioProcessorEditor (*this, parameters);
}

void AnalogDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void AnalogDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AnalogDelayAudioProcessor();
}
```

```cpp