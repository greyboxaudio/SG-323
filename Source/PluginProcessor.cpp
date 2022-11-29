/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SG323AudioProcessor::SG323AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

SG323AudioProcessor::~SG323AudioProcessor()
{
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
}

const juce::String SG323AudioProcessor::getProgramName(int index)
{
    return {};
}

void SG323AudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void SG323AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto delayBufferSize = sampleRate * 0.512;
    // set up filters
    lastSampleRate = sampleRate;
    float smoothSlow{0.1f};
    float smoothFast{0.0005f};
    inputGainSmooth.reset(sampleRate, smoothFast);
    highPassSmooth.reset(sampleRate, smoothFast);
    lowPassSmooth.reset(sampleRate, smoothFast);
    predelaySmooth.reset(sampleRate, smoothSlow);
    decaySmooth.reset(sampleRate, smoothSlow);
    wetDrySmooth.reset(sampleRate, smoothFast);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    inputHighPass.prepare(spec);
    inputHighPass.reset();
    inputLowPass.prepare(spec);
    inputLowPass.reset();
    randomHighPass.prepare(spec);
    randomHighPass.reset();
    randomLowPass.prepare(spec);
    randomLowPass.reset();
    preEmphasis.prepare(spec);
    preEmphasis.reset();
    deEmphasis.prepare(spec);
    deEmphasis.reset();
    feedBackHighPass.prepare(spec);
    feedBackHighPass.reset();
    feedBackLowPass.prepare(spec);
    feedBackLowPass.reset();
    feedBackDip.prepare(spec);
    feedBackDip.reset();
    antiAliasFirstSection.prepare(spec);
    antiAliasFirstSection.reset();
    antiAliasSecondSection.prepare(spec);
    antiAliasSecondSection.reset();
    antiAliasThirdSection.prepare(spec);
    antiAliasThirdSection.reset();
    gainModule.prepare(spec);
    gainModule.reset();
    fractionalDelay.prepare(spec);
    fractionalDelay.reset();
    fractionalDelay.setMaximumDelayInSamples(delayBufferSize);
    if (sampleRate == 44100.0)
    {
        s1b0 = 1.0f;
        s1b1 = 1.4407613277435302734375f;
        s1b2 = 1.0f;
        s1a0 = 1.0f;
        s1a1 = 0.78177297115325927734375f;
        s1a2 = 0.73473322391510009765625f;
        s1gain = 1.2458820343017578125f;
        s2b0 = 1.0f;
        s2b1 = 1.88249099254608154296875f;
        s2b2 = 1.0f;
        s2a0 = 1.0f;
        s2a1 = 0.0306032598018646240234375f;
        s2a2 = 0.18833030760288238525390625f;
        s2gain = 4.228640079498291015625f;
        s3b0 = 1.0f;
        s3b1 = 1.25116384029388427734375f;
        s3b2 = 1.0f;
        s3a0 = 1.0f;
        s3a1 = 1.0544550418853759765625f;
        s3a2 = 0.9471471309661865234375f;
        s3gain = 0.037988282740116119384765625f;
        modRateCeiling = 22;
        modScale = 1.378125f;
    }
    if (sampleRate == 48000.0)
    {
        s1b0 = 1.0f;
        s1b1 = 1.224283695220947265625f;
        s1b2 = 1.0f;
        s1a0 = 1.0f;
        s1a1 = 0.4837161600589752197265625f;
        s1a2 = 0.717677175998687744140625f;
        s1gain = 1.16304862499237060546875f;
        s2b0 = 1.0f;
        s2b1 = 1.828480243682861328125f;
        s2b2 = 1.0f;
        s2a0 = 1.0f;
        s2a1 = -0.2016279697418212890625f;
        s2a2 = 0.19512404501438140869140625f;
        s2gain = 3.4951908588409423828125f;
        s3b0 = 1.0f;
        s3b1 = 0.982987344264984130859375f;
        s3b2 = 1.0f;
        s3a0 = 1.0f;
        s3a1 = 0.7548847198486328125f;
        s3a2 = 0.942220151424407958984375f;
        s3gain = 0.0372033305466175079345703125f;
        modRateCeiling = 24;
        modScale = 1.5f;
    }
    if (sampleRate == 96000.0)
    {
        s1b0 = 1.0f;
        s1b1 = -0.4685294330120086669921875f;
        s1b2 = 1.0f;
        s1a0 = 1.0f;
        s1a1 = -1.01597499847412109375f;
        s1a2 = 0.635424196720123291015625f;
        s1gain = 0.73602163791656494140625f;
        s2b0 = 1.0f;
        s2b1 = 1.1502001285552978515625f;
        s2b2 = 1.0f;
        s2a0 = 1.0f;
        s2a1 = -0.972325623035430908203125f;
        s2a2 = 0.2985363900661468505859375f;
        s2gain = 1.00631582736968994140625f;
        s3b0 = 1.0f;
        s3b1 = -0.846724808216094970703125f;
        s3b2 = 1.0f;
        s3a0 = 1.0f;
        s3a1 = -1.07159888744354248046875f;
        s3a2 = 0.901829421520233154296875f;
        s3gain = 0.0404760427772998809814453125f;
        modRateCeiling = 48;
        modScale = 3.0f;
    }
}

void SG323AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SG323AudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

int calculateAddress(unsigned int rowInput, unsigned int columnInput)
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

int countWriteAddress(int writeAddress)
{
    // advance write address & wraparound if < 0
    int writeAddressIncr = writeAddress - 1;
    if (writeAddressIncr < 0)
    {
        writeAddressIncr = 16383;
    }
    return writeAddressIncr;
}

float roundBits(float inputSample)
{
    int roundedSample = inputSample * 32768;
    float outputSample = roundedSample * 0.000030518;
    return (outputSample);
}

int rngsus(float randomSample)
{
    int rateLevel{};
    int num1{};
    int num2{};
    int num3{};
    int num4{};
    if (randomSample >= 0.011f)
    {
        num1 = 1;
    }
    if (randomSample >= 0.0356f)
    {
        num2 = 2;
    }
    if (randomSample >= 0.0916f)
    {
        num3 = 4;
    }
    if (randomSample >= 0.317f)
    {
        num4 = 8;
    }
    rateLevel = num1 + num2 + num3 + num4;
    return (rateLevel);
}

void SG323AudioProcessor::updateFilter()
{
    *inputHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, nextHighPassValue);
    *inputLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, nextLowPassValue);
    *randomHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, 106.0f);
    *randomLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, 370.0f);
    *preEmphasis.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 4000.0f, 0.5f, 4.0f);
    *deEmphasis.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 4000.0f, 0.5f, 0.25f);
    *feedBackHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, 22.0f);
    *feedBackLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, 18000.0f);
    *feedBackDip.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(lastSampleRate, 9000.0f, 0.5f, 0.85f);
    *antiAliasFirstSection.state = juce::dsp::IIR::Coefficients<float>(s1b0, s1b1, s1b2, s1a0, s1a1, s1a2);
    *antiAliasSecondSection.state = juce::dsp::IIR::Coefficients<float>(s2b0, s2b1, s2b2, s2a0, s2a1, s2a2);
    *antiAliasThirdSection.state = juce::dsp::IIR::Coefficients<float>(s3b0, s3b1, s3b2, s3a0, s3a1, s3a2);
}

void SG323AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    auto bufferSize = buffer.getNumSamples();
    // read program selection from the UI
    int programId = *apvts.getRawParameterValue("PROGRAM");
    //read debug slider
    float debugValue = *apvts.getRawParameterValue("DEBUG");
    // prepare audio buffers
    monoBuffer.setSize(1, bufferSize);
    feedbackBuffer.setSize(1, bufferSize);
    randomBuffer.setSize(1, bufferSize);
    noiseBuffer.setSize(1, bufferSize);
    inputBuffer.setSize(totalNumInputChannels, bufferSize);
    outputBuffer.setSize(totalNumOutputChannels, bufferSize);
    if (initSampleRateCount == 0)
    {
        feedbackBuffer.clear(0, 0, buffer.getNumSamples());
        initSampleRateCount = 1;
    }
    // set up dsp elements
    juce::dsp::AudioBlock<float> monoBlock(monoBuffer);
    juce::dsp::AudioBlock<float> randomBlock(randomBuffer);
    juce::dsp::AudioBlock<float> feedbackBlock(feedbackBuffer);
    juce::dsp::AudioBlock<float> outputBlock(outputBuffer);
    // update filters
    float highPassValue = *apvts.getRawParameterValue("HPF");
    highPassSmooth.setTargetValue(highPassValue);
    nextHighPassValue = highPassSmooth.getNextValue();
    float lowPassValue = *apvts.getRawParameterValue("LPF");
    lowPassSmooth.setTargetValue(lowPassValue);
    nextLowPassValue = lowPassSmooth.getNextValue();
    updateFilter();
    // clear buffers
    monoBuffer.clear(0, 0, bufferSize);
    for (auto i = 0; i < totalNumOutputChannels; ++i)
        outputBuffer.clear(i, 0, bufferSize);
    // sum input channels together
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, bufferSize);
    monoBuffer.addFrom(0, 0, buffer, 1, 0, bufferSize);
    // apply input gain
    float inputGainValue = *apvts.getRawParameterValue("INPUT");
    inputGainSmooth.setTargetValue(inputGainValue * 0.5f);
    nextInputGainValue = inputGainSmooth.getNextValue();
    gainModule.setGainLinear(nextInputGainValue);
    gainModule.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    // copy & filter random Sample buffer
    randomBuffer.clear(0, 0, bufferSize);
    randomBuffer.copyFrom(0, 0, monoBuffer, 0, 0, bufferSize);
    randomHighPass.process(juce::dsp::ProcessContextReplacing<float>(randomBlock));
    randomLowPass.process(juce::dsp::ProcessContextReplacing<float>(randomBlock));
    // pre-process input buffer
    preEmphasis.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    inputHighPass.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    inputLowPass.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    // pre-process feedback buffer
    feedBackHighPass.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
    feedBackLowPass.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
    feedBackDip.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
    // sum input buffer & feedback buffer together
    monoBuffer.addFrom(0, 0, feedbackBuffer, 0, 0, bufferSize);
    // generate white noise
    float noiseLevel = 0.000125f;
    float noiseLevelHalf = 0.0000625f;
    for (int sample = 0; sample < bufferSize; ++sample)
    {
        float noiseSample = (random.nextFloat() * noiseLevel) - noiseLevelHalf;
        noiseBuffer.setSample(0, sample, noiseSample);
    }
    //sum input buffer & noise buffer together
    monoBuffer.addFrom(0, 0, noiseBuffer, 0, 0, bufferSize);
    // apply anti-aliasing filter
    gainModule.setGainLinear(s1gain);
    gainModule.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    antiAliasFirstSection.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    gainModule.setGainLinear(s2gain);
    gainModule.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    antiAliasSecondSection.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    gainModule.setGainLinear(s3gain);
    gainModule.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    antiAliasThirdSection.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    // //pre-scale input to create 12dB of headroom before clipping
    // gainModule.setGainLinear(0.25f);
    // gainModule.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
    // //quantize samples to 16bit
    // for (int i = 0; i < bufferSize; ++i)
    // {
    //     float sampleRounded = monoBuffer.getSample(0, i);
    //     monoBuffer.setSample(0, i, roundBits(sampleRounded));
    // }
    // //scale 16bit signal level back up
    // gainModule.setGainLinear(4.0f);
    // gainModule.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
    auto *data = monoBuffer.getReadPointer(0);
    // calculate the delay taps
    for (int i = 0; i < bufferSize; i++)
    {
        fractionalDelay.pushSample(0, data[i]);
        // calculate base address factors
        unsigned int decayTime = 7;
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
        float feedbackOutputSample{};
        float feedbackDelayTime{};
        float feedbackDelayGain{};
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
            if (gainModOut < gainOut && nGainModEnable == 0)
            {
                gainCeiling[1 + d] = gainModOut;
            }
            else
            {
                gainCeiling[1 + d] = gainOut;
            }
            unsigned int nGSN = gainData[gainAddress + d] >> 7;
            long readPosition = delayTaps[1 + d];
            float feedbackGain{};
            if (nGSN == 0)
            {
                feedbackGain = gainCeiling[1 + d] * -0.00390625f;
            }
            else
            {
                feedbackGain = gainCeiling[1 + d] * 0.00390625f;
            }
            int writeIndex = writeAddressArray[writePosition];
            int readIndex = writeAddressArray[readPosition];
            feedbackDelayTime = writeIndex - readIndex;
            if (feedbackDelayTime < 1)
            {
                feedbackDelayTime += 16384;
            }
            adjustableDecay = *apvts.getRawParameterValue("DECAY");
            decaySmooth.setTargetValue(adjustableDecay);
            float nextDecayValue = decaySmooth.getNextValue();
            feedbackDelayTime *= 0.00003125f;
            feedbackDelayGain = feedbackGain * (feedbackDelayGainMult * nextDecayValue);
            feedbackOutputSample += fractionalDelay.popSample(0, feedbackDelayTime * lastSampleRate, false) * feedbackDelayGain;
        }
        feedbackBuffer.setSample(0, i, feedbackOutputSample);
        // process random sample
        float randomSample = randomBuffer.getSample(0, i);
        if (randomSample < 0)
        {
            randomSample *= -0.33f;
        }
        // scale randomSample by a certain amount
        float randomSampleMult = 8.0f * debugValue;
        randomSample *= randomSampleMult;
        // calculate rateLVL value
        unsigned int rateLevel = rngsus(randomSample);
        // mod rate counter
        modClockOut += 1;
        if (modClockOut == modRateCeiling)
        {
            unsigned int modRateCount = rateLevel | (programId << 4);
            modClockOut = static_cast<int>(modRateCountData[modRateCount] * modScale);
        }
        unsigned int modCarry = modClockOut + 1;
        if (modCarry >= modRateCeiling)
        {
            MCCK = 1;
        }
        else
        {
            MCCK = 0;
        }
        nROW = countWriteAddress(writeAddress) & 255;
        nCOLUMN = countWriteAddress(writeAddress) >> 8;
        writeAddress = countWriteAddress(writeAddress);
        if (MCCK == 1)
        {
            modCount += 1;
            if (modCount > 8191)
            {
                modCount = 0;
            }
            gainModContBaseAddr = (modCount >> 1) & 4064;
            gainModBaseAddr = (modCount & 511) << 3;
            delayModBaseAddr = (modCount >> 1) & 4064;
        }
        // calculate output taps
        float outputDelayGainMult = 0.5f;
        float leftOutputSample{};
        float rightOutputSample{};
        float outputDelayTime{};
        float outputDelayGain{};
        // left output taps
        adjustablePreDelay = *apvts.getRawParameterValue("PREDELAY");
        predelaySmooth.setTargetValue(adjustablePreDelay);
        float nextPreDelayValue = predelaySmooth.getNextValue();
        for (int d = 0; d < 4; d++)
        {
            outputDelayTime = ((programId * outputDelayArray[d]) + outputDelayArray[d + 8] + nextPreDelayValue) * 0.001f;
            outputDelayGain = outputGainArray[d] * outputDelayGainMult;
            leftOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, false) * outputDelayGain;
        }
        // right output taps
        for (int d = 4; d < 7; d++)
        {
            outputDelayTime = ((programId * outputDelayArray[d]) + outputDelayArray[d + 8] + nextPreDelayValue) * 0.001f;
            outputDelayGain = outputGainArray[d] * outputDelayGainMult;
            rightOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, false) * outputDelayGain;
        }
        outputDelayTime = ((programId * outputDelayArray[7]) + outputDelayArray[7 + 8] + nextPreDelayValue) * 0.001f;
        outputDelayGain = outputGainArray[7] * outputDelayGainMult;
        rightOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, true) * outputDelayGain;
        outputBuffer.setSample(0, i, leftOutputSample);
        outputBuffer.setSample(1, i, rightOutputSample);
    }
    // apply de-emphasis filter on output taps
    deEmphasis.process(juce::dsp::ProcessContextReplacing<float>(outputBlock));
    // copy dry signal to buffer
    for (int channel = 0; channel < totalNumInputChannels; channel++)
    {
        inputBuffer.copyFrom(channel, 0, buffer, channel, 0, bufferSize);
    }
    // write output taps to main buffer
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto *wetSignal = outputBuffer.getReadPointer(channel);
        auto *drySignal = inputBuffer.getReadPointer(channel);
        float mixLevel = *apvts.getRawParameterValue("WETDRY");
        wetDrySmooth.setTargetValue(mixLevel);
        float wetLevel = wetDrySmooth.getNextValue();
        float dryLevel = 1.0f - wetLevel;
        buffer.copyFromWithRamp(channel, 0, drySignal, bufferSize, dryLevel, dryLevel);
        buffer.addFromWithRamp(channel, 0, wetSignal, bufferSize, mixLevel, mixLevel);
    }
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
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterChoice>("PROGRAM", "Program",
                                                                      juce::StringArray("Plate 1", "Plate 2", "Chamber", "Small Hall", "Hall", "Large Hall", "Cathedral", "Canyon"), 0));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("PREDELAY", "PreDelay", 0.0f, 320.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", 0.0f, 1.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("WETDRY", "WetDry", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("HPF", "highPassFilter", 20.0f, 480.0f, 20.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LPF", "lowPassFilter", 3000.0f, 16000.0f, 16000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("INPUT", "inputGain", 0.0f, 2.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DEBUG", "debug", 0.0f, 2.0f, 1.0f));
    return {parameters.begin(), parameters.end()};
}