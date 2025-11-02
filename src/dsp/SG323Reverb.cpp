#pragma once

#include "SG323Reverb.h"

namespace SG323
{

Reverb::Reverb()
{
    mixer.setMixingRule(juce::dsp::DryWetMixingRule::sin3dB); // this is a change from the original design, but I think this mode is better for reverbs
}

void Reverb::setProgramId(int id)
{
    jassert(juce::isPositiveAndNotGreaterThan(id, NUM_PROGRAMS));

    programId = juce::jlimit(0, NUM_PROGRAMS, id);
}

void Reverb::setPreDelay(float delayMilliseconds)
{
    jassert(delayMilliseconds >= 0.0f);
    preDelaySmooth.setTargetValue(juce::jlimit(0.0f, 320.0f, delayMilliseconds));
}

void Reverb::setDecay(float decayPercent)
{
    jassert(juce::isPositiveAndNotGreaterThan(decayPercent, 100.0f));
    decaySmooth.setTargetValue(juce::jlimit(0.0f, 100.0f, decayPercent) * 0.01f);
}

void Reverb::setMix(float wetMixPercent)
{
    jassert(juce::isPositiveAndNotGreaterThan(wetMixPercent, 100.0f));
    mixer.setWetMixProportion(juce::jlimit(0.0f, 100.0f, wetMixPercent) * 0.01f);
}

void Reverb::setLowDecay(float frequency)
{
    jassert(frequency > 0.0f);
    highPassSmooth.setTargetValue(juce::jlimit(20.0f, 480.0f, frequency));

}

void Reverb::setHighDecay(float frequency)
{
    jassert(frequency > 0.0f);
    lowPassSmooth.setTargetValue(juce::jlimit(3000.0f, 16000.0f, frequency));
}

void Reverb::setInputGain(float gainLevel)
{
    jassert(juce::isPositiveAndNotGreaterThan(gainLevel, 2.0f));
    inputGainSmooth.setTargetValue(juce::jlimit(0.0f, 2.0f, gainLevel));
}

void Reverb::setVintageMode(bool shouldUseVinageMode)
{
    vintageMode = shouldUseVinageMode;
}

void Reverb::setNoise(bool shouldAddNoise)
{
    addNoise = shouldAddNoise;
}

void Reverb::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    fSampleRate = static_cast<float>(sampleRate);
    auto delayBufferSize = static_cast<int>(sampleRate * 0.512);

    const auto smoothSlow = 0.1;
    const auto smoothFast = 0.0005;

    inputGainSmooth.reset(sampleRate, smoothFast);
    highPassSmooth.reset(sampleRate, smoothFast);
    lowPassSmooth.reset(sampleRate, smoothFast);
    preDelaySmooth.reset(sampleRate, smoothSlow);
    decaySmooth.reset(sampleRate, smoothSlow);

    mixer.prepare(spec);
    
    auto monoSpec = spec;
    monoSpec.numChannels = 1;

    for(auto& f : filters) {
        f.prepare(monoSpec);
    }

    for(auto& f : aaFilters) {
        f.prepare(monoSpec);
    }

    for(auto& f : fbkFilters) {
        f.prepare(monoSpec);
    }

    fractionalDelay.prepare(monoSpec);
    fractionalDelay.setMaximumDelayInSamples(delayBufferSize);

    modScale = fSampleRate * 0.00003125f;
    modRateCeiling = static_cast<int>(16.0f * modScale);

    initializeFilters();

    const auto bufferSize = int(spec.maximumBlockSize);
    monoBuffer.setSize(1, bufferSize, false, true);
    randomBuffer.setSize(1, bufferSize, false, true);
}

void Reverb::initializeFilters()
{
    *(filters[FILTER_ID::inputHighPass].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, highPassSmooth.getCurrentValue());
    *(filters[FILTER_ID::inputLowPass].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, lowPassSmooth.getCurrentValue());

    *(filters[FILTER_ID::randomHighPass].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, 106.0f);
    *(filters[FILTER_ID::randomLowPass].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, 370.0f);

    *(filters[FILTER_ID::preEmphasis].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 4000.0f, 0.5f, 4.0f);
    *(filters[FILTER_ID::deEmphasisL].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 4000.0f, 0.5f, 0.25f);
    *(filters[FILTER_ID::deEmphasisR].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 4000.0f, 0.5f, 0.25f);

    *(fbkFilters[0].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, 22.0f);
    *(fbkFilters[1].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, 18000.0f);
    *(fbkFilters[2].coefficients) = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 9000.0f, 0.5f, 0.85f);

    const auto aaFrequency = std::min(fSampleRate * 0.45f, 19500.0f);
    for(auto& f : aaFilters)
    {
        f.setCutoffFrequency(aaFrequency);
        f.setResonance(0.707f);
        f.reset();
    }

    for(auto& f : filters) {
        f.reset();
    }

    for(auto& f : filters) {
        f.reset();
    }
}

void Reverb::updateInputFilters()
{
    *(filters[FILTER_ID::inputHighPass].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, highPassSmooth.getNextValue());
    *(filters[FILTER_ID::inputLowPass].coefficients) = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, lowPassSmooth.getNextValue());
}

int Reverb::countWriteAddress()
{
    writeAddress -= 1;
    if (writeAddress < 0) {
        writeAddress = 16383;
    }
    return writeAddress;
}

int Reverb::calculateAddress(unsigned int rowInput, unsigned int columnInput)
{
    // calculate address row
    unsigned int bit6 = (rowInput & 64) >> 6;
    unsigned int MSB = (rowInput & 128) >> 7;
    unsigned int delayCarryOut = rowInput >> 8;
    unsigned int rowDelay = ((rowInput << 1) & 126) | bit6 | (MSB << 7);
    // calculate address column
    unsigned int columnDelay = (columnInput + delayCarryOut) & 63;
    return ((rowDelay) + (columnDelay * 256));
}

int Reverb::rngsus(float randomSample)
{
    int rateLevel = 0;

    if (randomSample >= 0.011f) {
        rateLevel += 1;
    }
    if (randomSample >= 0.0356f) {
        rateLevel += 2;
    }
    if (randomSample >= 0.0916f) {
        rateLevel += 4;
    }
    if (randomSample >= 0.317f) {
        rateLevel += 8;
    }

    return (rateLevel);
}

void Reverb::processBuffer(juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = buffer.getNumChannels();
    auto mainBlock = juce::dsp::AudioBlock<float>(buffer);

    mixer.pushDrySamples(mainBlock);

    monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
    if(numChannels == 2) {
        monoBuffer.addFrom(0, 0, buffer, 1, 0, numSamples);
        monoBuffer.applyGain(0.5f);
    }
    inputGainSmooth.applyGain(monoBuffer, numSamples);
    auto monoBlock = juce::dsp::AudioBlock<float>(monoBuffer).getSubBlock(0, numSamples);
    auto monoContext = juce::dsp::ProcessContextReplacing<float>(monoBlock);

    randomBuffer.copyFrom(0, 0, monoBuffer, 0, 0, numSamples);
    auto randomBlock = juce::dsp::AudioBlock<float>(randomBuffer).getSubBlock(0, numSamples);
    auto randomContext = juce::dsp::ProcessContextReplacing<float>(randomBlock);
    filters[FILTER_ID::randomHighPass].process(randomContext);
    filters[FILTER_ID::randomLowPass].process(randomContext);

    filters[FILTER_ID::preEmphasis].process(monoContext);

    // calculate & process the delay taps
    auto *data = monoBuffer.getWritePointer(0);
    unsigned int decayTime = 7;
    for (int i = 0; i < numSamples; i++)
    {
        if(highPassSmooth.isSmoothing() || lowPassSmooth.isSmoothing()) {
            updateInputFilters();
        }
        data[i] = filters[FILTER_ID::inputHighPass].processSample(data[i]);
        data[i] = filters[FILTER_ID::inputLowPass].processSample(data[i]);
        // calculate base address factors
        unsigned int gainBaseAddr = (decayTime << 5) | (programId << 8);
        unsigned int delayBaseAddr = programId << 6;
        // calculate write tap (=test tap)
        int rowInput = nROW;
        int columnInput = nCOLUMN;
        long writePosition = static_cast<int>(calculateAddress(rowInput, columnInput));
        // calculate feedback taps
        unsigned int delayModAddress = delayModBaseAddr + 7;
        unsigned int delayAddress = delayBaseAddr + 16;
        unsigned int gainModContAddress = gainModContBaseAddr + 8;
        unsigned int gainAddress = gainBaseAddr + 8;
        float feedbackDelayGainMult = -0.29f;
        float feedbackOutputSample = 0.0f;
        for (int d = 0; d < 15; d++)
        {
            rowInput = delayModData[delayModAddress + d] + nROW;
            columnInput = delayData[delayAddress + d * 2] + nCOLUMN;
            delayTaps[1 + d] = calculateAddress(rowInput, columnInput);
            unsigned int gainModContOut = gainModControlData[gainModContAddress + d] & 7;
            unsigned int nGainModEnable = gainModControlData[gainModContAddress + d] >> 3;
            unsigned int gainModAddress = gainModContOut | gainModBaseAddr;
            unsigned int gainModOut = gainModData[gainModAddress];
            unsigned int gainOut = (gainData[gainAddress + d] << 1) & 255;
            if (gainModOut < gainOut && nGainModEnable == 0) {
                gainCeiling[1 + d] = gainModOut;
            }
            else {
                gainCeiling[1 + d] = gainOut;
            }
            unsigned int nGSN = gainData[gainAddress + d] >> 7;
            long readPosition = delayTaps[1 + d];
            float feedbackGain{};
            if (nGSN == 0) {
                feedbackGain = gainCeiling[1 + d] * -0.00390625f;
            }
            else {
                feedbackGain = gainCeiling[1 + d] * 0.00390625f;
            }
            int writeIndex = writeAddressArray[writePosition];
            int readIndex = writeAddressArray[readPosition];
            auto feedbackDelayTime = static_cast<float>(writeIndex - readIndex);
            if (feedbackDelayTime < 1) {
                feedbackDelayTime += 16384;
            }

            float nextDecayValue = d == 0 ? decaySmooth.getNextValue() : decaySmooth.getCurrentValue();
            feedbackDelayTime *= 0.00003125f;
            auto feedbackDelayGain = feedbackGain * (feedbackDelayGainMult * nextDecayValue);
            feedbackOutputSample += fractionalDelay.popSample(0, feedbackDelayTime * fSampleRate, false) * feedbackDelayGain;
        }
        for(auto& f : fbkFilters) {
            feedbackOutputSample = f.processSample(feedbackOutputSample);
        }

        auto input = data[i] + feedbackOutputSample;
        if(vintageMode) {
            input = std::floor(input * 32768.0f) * 0.000030518f;
        }
        if(addNoise) {
            input += ((random.nextFloat() * 2.0f) - 1.0f) * NOISE_LEVEL;
        }
        for(auto& f : aaFilters) {
            input = f.processSample(0, input);
        }

        fractionalDelay.pushSample(0, input);

        // process random sample
        modClockOut += 1;
        if (modClockOut == modRateCeiling)
        {
            float randomSample = randomBuffer.getSample(0, i);
            if (randomSample < 0) {
                randomSample *= -0.33f;
            }
            randomSample *= 8.0f;
            unsigned int rateLevel = rngsus(randomSample);
            unsigned int modRateCount = rateLevel | (programId << 4);
            modClockOut = static_cast<int>(modRateCountData[modRateCount] * modScale);
        }
        MCCK = (modClockOut + 1) >= modRateCeiling ? 1 : 0;
        nROW = countWriteAddress() & 255;
        nCOLUMN = countWriteAddress() >> 8;
        writeAddress = countWriteAddress();
        if (MCCK == 1)
        {
            modCount += 1;
            if (modCount > 8191) {
                modCount = 0;
            }
            gainModContBaseAddr = (modCount >> 1) & 4064;
            gainModBaseAddr = (modCount & 511) << 3;
            delayModBaseAddr = (modCount >> 1) & 4064;
        }

        // calculate output taps
        float nextPreDelayValue = preDelaySmooth.getNextValue();
        
        auto leftOutputSample = 0.0f;
        for (int d = 0; d < 4; d++)
        {
            auto outputDelayTime = ((programId * outputDelayArray[d]) + outputDelayArray[d + 8] + nextPreDelayValue) * 0.001f;
            leftOutputSample += fractionalDelay.popSample(0, outputDelayTime * fSampleRate, false) * outputGainArray[d] * 0.5f;
        }

        auto rightOutputSample = 0.0f;
        for (int d = 4; d < 8; d++)
        {
            auto outputDelayTime = ((programId * outputDelayArray[d]) + outputDelayArray[d + 8] + nextPreDelayValue) * 0.001f;
            rightOutputSample += fractionalDelay.popSample(0, outputDelayTime * fSampleRate, d == 7) * outputGainArray[d] * 0.5f;
        }

        if(vintageMode)
        {
            leftOutputSample = std::floor(leftOutputSample * 32768.0f) * 0.000030518f;
            rightOutputSample = std::floor(rightOutputSample * 32768.0f) * 0.000030518f;
        }

        if (numChannels == 1) {
            auto output = filters[FILTER_ID::deEmphasisL].processSample(leftOutputSample + rightOutputSample);
            buffer.setSample(0, i, output);
        }
        else {
            buffer.setSample(0, i, filters[FILTER_ID::deEmphasisL].processSample(leftOutputSample));
            buffer.setSample(1, i, filters[FILTER_ID::deEmphasisR].processSample(rightOutputSample));
        }
    }

    mixer.mixWetSamples(mainBlock);
}

}