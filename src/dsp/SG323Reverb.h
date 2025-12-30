#pragma once

#include "SG323ReverbData.h"
#include <juce_dsp/juce_dsp.h>

namespace SG323
{

class Reverb
{
public:

    Reverb();
    ~Reverb() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void processBuffer(juce::AudioBuffer<float>& buffer);

    void setProgramId(int id);
    void setPreDelay(float delayMilliseconds);
    void setDecay(float decayPercent);
    void setMix(float wetMixPercent);
    void setLowDecay(float frequency);
    void setHighDecay(float frequency);
    void setInputGain(float gainLevel);
    void setVintageMode(bool shouldUseVinageMode);
    void setNoise(bool shouldAddNoise);

private:

    void initializeFilters();
    void updateInputFilters();
    int calculateAddress(unsigned int rowInput, unsigned int columnInput);
    int rngsus(float randomSample);
    int countWriteAddress();
    inline float quantizeSample(float sample);

    juce::AudioBuffer<float> monoBuffer;
    juce::AudioBuffer<float> randomBuffer;
    
    static constexpr float NOISE_LEVEL = 0.00012f;

    juce::Random random;

    juce::LinearSmoothedValue<float> inputGainSmooth { 1.0f },
                                     highPassSmooth  { 20.0f },
                                     lowPassSmooth   { 16000.0f },
                                     preDelaySmooth  { 0.0f },
                                     decaySmooth     { 0.7f };
    bool vintageMode { true }, addNoise { false };

    juce::dsp::Gain<float> gainModule;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> fractionalDelay;
    juce::dsp::DryWetMixer<float> mixer;

    enum FILTER_ID
    {
        inputHighPass, inputLowPass, 
        randomHighPass, randomLowPass, 
        preEmphasis, deEmphasisL, deEmphasisR,
        numFilters
    };

    std::array<juce::dsp::IIR::Filter<float>, numFilters> filters;
    std::vector<juce::dsp::IIR::Filter<float>> aaFilters;
    std::array<juce::dsp::IIR::Filter<float>, 3> fbkFilters;

    float outputGainArray[8] {-0.996f, 0.996f, 0.622f, 0.378f, 0.378f, 0.622f, 0.966f, -0.966f};
    float outputDelayArray[16] {10.5f, 7.0f, 3.5f, 0.0f, 19.0f, 13.0f, 7.0f, 1.0f, 11.5f, 8.0f, 4.5f, 1.0f, 20.0f, 14.0f, 8.0f, 2.0f};
    unsigned long delayTaps[24];
    unsigned int gainCeiling[24];
    int initSampleRateCount{};

    double sampleRate = 48000.0;
    float fSampleRate = static_cast<float>(sampleRate);

    int programId {3};
    int writeAddress {16383};
    unsigned int nROW {255};
    unsigned int nCOLUMN {255};
    unsigned int modRateCeiling {16};
    float modScale {1.0f};
    unsigned int MCCK {0};
    unsigned int modClockOut {0};
    unsigned int modCount {0};
    unsigned int gainModContBaseAddr {0};
    unsigned int gainModBaseAddr {0};
    unsigned int delayModBaseAddr {0};

};

}
