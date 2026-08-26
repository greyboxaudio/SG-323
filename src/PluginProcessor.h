/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 */
class SG323AudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                            public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    SG323AudioProcessor();
    ~SG323AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    void updateFilter();
    juce::AudioProcessorValueTreeState apvts;
    std::atomic<bool> clearButtonState;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    juce::AudioBuffer<float> inputBuffer;
    juce::AudioBuffer<float> monoBuffer;
    juce::AudioBuffer<float> randomBuffer;
    juce::AudioBuffer<float> feedbackBuffer;
    juce::AudioBuffer<float> outputBuffer;
    juce::AudioBuffer<float> noiseBuffer;
    juce::AudioBuffer<float> bitBuffer;

    const int fifoBufferSize = 8192;
    juce::AbstractFifo abstractFifo{fifoBufferSize};
    int start1, size1, start2, size2;
    juce::AudioBuffer<float> fifoBuffer;

    juce::Random random;

    juce::LinearSmoothedValue<float> inputGainSmooth{};
    juce::LinearSmoothedValue<float> highPassSmooth{};
    juce::LinearSmoothedValue<float> lowPassSmooth{};
    juce::LinearSmoothedValue<float> predelaySmooth{};
    juce::LinearSmoothedValue<float> decaySmooth{};
    juce::LinearSmoothedValue<float> mixSmooth{};

    juce::dsp::Gain<float> gainModule;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> fractionalDelay;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> inputHighPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> inputLowPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> randomHighPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> randomLowPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> preEmphasis;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> deEmphasis;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> feedBackDip;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> feedBackHighPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> feedBackLowPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> antiAliasFirstSection;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> antiAliasSecondSection;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> antiAliasThirdSection;

    float outputGainArray[8]{};
    float outputDelayArray[8]{};
    unsigned long delayTaps[24];
    unsigned int gainCeiling[24];
    float lastSampleRate{};
    float nextHighPassValue{};
    float nextLowPassValue{};
    int sgLookup[16]{3, 2, 0, 1, 5, 4, 6, 7, 11, 10, 8, 9, 13, 12, 14, 15};
    int writeAddress{65535};
    int writeAddressArray[65536]{};
    unsigned int nROW{255};
    unsigned int nCOLUMN{255};
    unsigned int modRateCeiling{16};
    float modScale{1.0f};
    unsigned int MCCK{};
    unsigned int modClockOut{};
    unsigned int modCount{};
    unsigned int gainModContBaseAddr{};
    unsigned int gainModBaseAddr{};
    unsigned int delayModBaseAddr{};
    bool moddis;
    unsigned int rateLevel{};
    unsigned int decayTime{15};
    unsigned int preDelay{3};

    float preDelayMod[144]{0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.0f,0.2415f,0.1680f,0.0945f,0.0210f,0.4200f,0.2940f,0.1680f,0.0210f,1.952f,1.9500f,1.9500f,1.9500f,1.9500f,1.9500f,1.9500f,1.9500f,1.9500f,1.952f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.952f,0.9750f,0.9750f,0.9750f,0.9750f,1.9500f,1.9500f,1.9500f,1.9500f,1.800f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f,0.3200f};
    float s1a0{1.0f};
    float s1a1{1.0f};
    float s1a2{1.0f};
    float s1b0{1.0f};
    float s1b1{1.0f};
    float s1b2{1.0f};
    float s2a0{1.0f};
    float s2a1{1.0f};
    float s2a2{1.0f};
    float s2b0{1.0f};
    float s2b1{1.0f};
    float s2b2{1.0f};
    float s3a0{1.0f};
    float s3a1{1.0f};
    float s3a2{1.0f};
    float s3b0{1.0f};
    float s3b1{1.0f};
    float s3b2{1.0f};
    double ellipticCoefficients[108]{0.1590722219519817, 0.2925800084337282, 0.15907222195198167, 1.0, -0.1616482402221111, 0.19263764096311883, 1.0, 1.265871938992507, 0.9999999999999998, 1.0, 0.5376549648832389, 0.7201150300473758, 1.0, 1.0336348451847246, 1.0, 1.0, 0.8104458709784527, 0.9429830293245048, 0.12217156278343735, 0.21708074800925328, 0.12217156278343735, 1.0, -0.3622982289466266, 0.21069044780090654, 1.0, 1.036555560330608, 1.0000000000000002, 1.0, 0.2564381854840234, 0.7103381856084345, 1.0, 0.759333766447034, 0.9999999999999998, 1.0, 0.5147161937185064, 0.939618968627594, 0.03001113166367873, 0.02967221178596468, 0.030011131663678737, 1.0, -1.196963794847673, 0.44585426837637376, 1.0, -0.5800931166584409, 0.9999999999999999, 1.0, -1.0890277974018174, 0.7625980831299516, 1.0, -0.8811254991797578, 1.0000000000000002, 1.0, -1.0466322049865133, 0.9469912658394941, 0.026152742312366212, 0.021748966292658066, 0.02615274231236622, 1.0, -1.2666830304156562, 0.4787076012745491, 1.0, -0.7561492861543417, 1.0, 1.0, -1.1994214057403096, 0.7757059495161946, 1.0, -1.0338334903782682, 1.0, 1.0, -1.1783179130907315, 0.9498397819567047, 0.01345196428348894, -0.00578353415575715, 0.01345196428348894, 1.0, -1.609978347980477, 0.677670924022425, 1.0, -1.570407382335601, 0.9999999999999998, 1.0, -1.6792585172217143, 0.8621444371857153, 1.0, -1.6869584422150632, 0.9999999999999999, 1.0, -1.7263819265125393, 0.9694005278843073, 0.012734638654465117, -0.007569262219551063, 0.012734638654465117, 1.0, -1.6422421825069187, 0.6999451424751979, 1.0, -1.6334218277265786, 0.9999999999999997, 1.0, -1.7166739247723046, 0.8720771412216309, 1.0, -1.7341496621089312, 1.0000000000000002, 1.0, -1.7656027882101546, 0.9716653621519904};
};