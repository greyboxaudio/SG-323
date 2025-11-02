/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SG323AudioProcessor::SG323AudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameters())
{
    for (auto* param : getParameters()) {
        param->addListener(this);
    }
}

SG323AudioProcessor::~SG323AudioProcessor()
{
    for (auto* param : getParameters()) {
        param->removeListener(this);
    }
}

//==============================================================================
const juce::String SG323AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SG323AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SG323AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SG323AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SG323AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SG323AudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int SG323AudioProcessor::getCurrentProgram()
{
    return 0;
}

void SG323AudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String SG323AudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void SG323AudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void SG323AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    reverbProcessor.prepare(spec);
}

void SG323AudioProcessor::releaseResources()
{
}

bool SG323AudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    // update to add mono->stereo support

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void SG323AudioProcessor::parameterValueChanged (int parameterIndex, float newValue)
{
    juce::ignoreUnused(parameterIndex, newValue);
    updateFlag.store(true);
}

void SG323AudioProcessor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
{
    juce::ignoreUnused(parameterIndex, gestureIsStarting);
}

void SG323AudioProcessor::updateParameters()
{
    auto programId = static_cast<int>(apvts.getRawParameterValue("PROGRAM")->load());
    auto inputGainValue = apvts.getRawParameterValue("INPUT")->load();
    auto lfdecayValue = apvts.getRawParameterValue("LFDECAY")->load();
    auto hfdecayValue = apvts.getRawParameterValue("HFDECAY")->load();
    auto mixLevel = apvts.getRawParameterValue("MIX")->load();
    auto adjustablePreDelay = apvts.getRawParameterValue("PREDELAY")->load();
    auto adjustableDecay = apvts.getRawParameterValue("DECAY")->load();

    reverbProcessor.setProgramId(programId);
    reverbProcessor.setInputGain(inputGainValue);
    reverbProcessor.setLowDecay(lfdecayValue);
    reverbProcessor.setHighDecay(hfdecayValue);
    reverbProcessor.setMix(mixLevel);
    reverbProcessor.setPreDelay(adjustablePreDelay);
    reverbProcessor.setDecay(adjustableDecay);
}

void SG323AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);

    if(updateFlag.exchange(false)) {
        updateParameters();
    }

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    
    reverbProcessor.processBuffer(buffer);
    
}

//==============================================================================
bool SG323AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *SG323AudioProcessor::createEditor()
{
    return new SG323AudioProcessorEditor(*this);
}

//==============================================================================
void SG323AudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SG323AudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new SG323AudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SG323AudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;

    parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("PROGRAM", 1), "Program", SG323::programNames, 3));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PREDELAY", 1), "Pre Delay", 0.0f, 320.0f, 0.0f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DECAY", 1), "Decay", 0.0f, 100.0f, 70.0f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MIX", 1), "Mix", 0.0f, 100.0f, 50.0f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("LFDECAY", 1), "Low Frequency Decay", juce::NormalisableRange<float>(20.0f, 480.0f, 0.01f, 0.5f), 250.0f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("HFDECAY", 1), "High Frequency Decay", juce::NormalisableRange<float>(3000.0f, 16000.0f, 0.01f, 0.33f), 6000.0f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("INPUT", 1), "Input Gain", 0.0f, 2.0f, 1.0f));
    parameters.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("VINTAGE", 1), "Vintage", true));
    parameters.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("NOISE", 1), "Noise", true));

    return parameters;
}