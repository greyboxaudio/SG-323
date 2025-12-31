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
int calculateAddress(unsigned int rowInput, unsigned int columnInput)
{
    // calculate address row
    unsigned int bit6 = (rowInput & 0x40) >> 6;
    unsigned int MSB = (rowInput & 0x80) >> 7;
    unsigned int delayCarryOut = rowInput >> 8;
    unsigned int rowDelay = ((rowInput & 0x3f) << 1) | bit6 | (MSB << 7);
    // calculate address column
    bit6 = ((columnInput + delayCarryOut) & 0x40) >> 6;
    MSB = ((columnInput + delayCarryOut) & 0x80) >> 7;
    unsigned int columnDelay = (((columnInput + delayCarryOut) & 0x3f) << 1) | bit6 | (MSB << 7);
    return ((rowDelay) + (columnDelay * 256));
}

int countWriteAddress(int writeAddress)
{
    // advance write address & wraparound if < 0
    int writeAddressIncr = writeAddress - 1;
    if (writeAddressIncr < 0)
    {
        writeAddressIncr = 65535;
    }
    return writeAddressIncr;
}

float roundBits(float inputSample)
{
    int roundedSample = static_cast<int>(inputSample * 32768);
    float outputSample = static_cast<float>(roundedSample * 0.000030518);
    return (outputSample);
}

//==============================================================================
void SG323AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto delayBufferSize = sampleRate * 1.024;
    // set up filters
    lastSampleRate = static_cast<float>(sampleRate);
    float smoothSlow{0.1f};
    float smoothFast{0.0005f};
    inputGainSmooth.reset(sampleRate, smoothFast);
    highPassSmooth.reset(sampleRate, smoothFast);
    lowPassSmooth.reset(sampleRate, smoothFast);
    predelaySmooth.reset(sampleRate, smoothSlow);
    decaySmooth.reset(sampleRate, smoothSlow);
    mixSmooth.reset(sampleRate, smoothFast);
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
    fractionalDelay.setMaximumDelayInSamples(static_cast<int>(delayBufferSize));
    modScale = lastSampleRate * 0.00003125f;
    modRateCeiling = static_cast<int>(16 * modScale);
    int IIR_sr{0};
    if (lastSampleRate == 48000.0)
    {
        IIR_sr = 1;
    }
    if (lastSampleRate == 88200.0)
    {
        IIR_sr = 2;
    }
    if (lastSampleRate == 96000.0)
    {
        IIR_sr = 3;
    }
    if (lastSampleRate == 176400.0)
    {
        IIR_sr = 4;
    }
    if (lastSampleRate == 192000.0)
    {
        IIR_sr = 5;
    }
    int IIR_base = IIR_sr * 18;
    s1b0 = static_cast<float>(ellipticCoefficients[IIR_base]);
    s1b1 = static_cast<float>(ellipticCoefficients[IIR_base + 1]);
    s1b2 = static_cast<float>(ellipticCoefficients[IIR_base + 2]);
    s1a0 = static_cast<float>(ellipticCoefficients[IIR_base + 3]);
    s1a1 = static_cast<float>(ellipticCoefficients[IIR_base + 4]);
    s1a2 = static_cast<float>(ellipticCoefficients[IIR_base + 5]);
    s2b0 = static_cast<float>(ellipticCoefficients[IIR_base + 6]);
    s2b1 = static_cast<float>(ellipticCoefficients[IIR_base + 7]);
    s2b2 = static_cast<float>(ellipticCoefficients[IIR_base + 8]);
    s2a0 = static_cast<float>(ellipticCoefficients[IIR_base + 9]);
    s2a1 = static_cast<float>(ellipticCoefficients[IIR_base + 10]);
    s2a2 = static_cast<float>(ellipticCoefficients[IIR_base + 11]);
    s3b0 = static_cast<float>(ellipticCoefficients[IIR_base + 12]);
    s3b1 = static_cast<float>(ellipticCoefficients[IIR_base + 13]);
    s3b2 = static_cast<float>(ellipticCoefficients[IIR_base + 14]);
    s3a0 = static_cast<float>(ellipticCoefficients[IIR_base + 15]);
    s3a1 = static_cast<float>(ellipticCoefficients[IIR_base + 16]);
    s3a2 = static_cast<float>(ellipticCoefficients[IIR_base + 17]);
    // store write address sequence
    for (int i = 0; i < 65535; i++)
    {
        int x = calculateAddress(nROW, nCOLUMN);
        writeAddressArray[x] = i;
        nROW = countWriteAddress(writeAddress) & 255;
        nCOLUMN = countWriteAddress(writeAddress) >> 8;
        writeAddress = countWriteAddress(writeAddress);
    };
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
#ifndef IS_DEMO
    float inputGainValue = *apvts.getRawParameterValue("INPUT");
    float lfdecayValue = *apvts.getRawParameterValue("LFDECAY");
    float hfdecayValue = *apvts.getRawParameterValue("HFDECAY");
    float mixLevel = *apvts.getRawParameterValue("MIX") * 0.01f;
    float adjustablePreDelay = *apvts.getRawParameterValue("PREDELAY");
    float adjustableDecay = *apvts.getRawParameterValue("DECAY") * 0.01f;
#else
    float inputGainValue = 1.0f;
    float lfdecayValue = 20.0f;
    float hfdecayValue = 16000.0f;
    float mixLevel = 1.0f;
    float adjustablePreDelay = 0.0f;
    float adjustableDecay = 0.7f;
#endif

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
    int programId = static_cast<int>(*apvts.getRawParameterValue("PROGRAM"));
    programId = sgLookup[programId];
    // prepare audio buffers
    monoBuffer.setSize(1, bufferSize);
    feedbackBuffer.setSize(1, bufferSize);
    randomBuffer.setSize(1, bufferSize);
    noiseBuffer.setSize(1, bufferSize);
    bitBuffer.setSize(1, bufferSize);
    fifoBuffer.setSize(1, fifoBufferSize);
    inputBuffer.setSize(totalNumInputChannels, bufferSize);
    outputBuffer.setSize(totalNumOutputChannels, bufferSize);
    if (initSampleRateCount == 0)
    {
        fifoBuffer.clear(0, 0, fifoBufferSize);
        initSampleRateCount = 1;
    }
    // set up dsp elements
    juce::dsp::AudioBlock<float> monoBlock(monoBuffer);
    juce::dsp::AudioBlock<float> randomBlock(randomBuffer);
    juce::dsp::AudioBlock<float> outputBlock(outputBuffer);
    // update filters
    highPassSmooth.setTargetValue(lfdecayValue);
    nextHighPassValue = highPassSmooth.getNextValue();
    lowPassSmooth.setTargetValue(hfdecayValue);
    nextLowPassValue = lowPassSmooth.getNextValue();
    updateFilter();
    // clear buffers
    monoBuffer.clear(0, 0, bufferSize);
    feedbackBuffer.clear(0, 0, bufferSize);
    for (auto i = 0; i < totalNumOutputChannels; ++i)
        outputBuffer.clear(i, 0, bufferSize);
    // add left channel to monoBuffer
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, bufferSize);
    // add right channel to monoBuffer when running in stereo
    if (totalNumInputChannels == 2)
    {
        monoBuffer.addFrom(0, 0, buffer, 1, 0, bufferSize);
        gainModule.setGainLinear(0.5);
        gainModule.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    }
    // apply input gain
    inputGainSmooth.setTargetValue(inputGainValue);
    float nextInputGainValue = inputGainSmooth.getNextValue();
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

    // sum input buffer & feedback buffer together
    abstractFifo.prepareToRead(bufferSize, start1, size1, start2, size2);
    if (size1 > 0)
    {
        monoBuffer.addFrom(0, 0, fifoBuffer, 0, start1, size1);
    }
    if (size2 > 0)
    {
        monoBuffer.addFrom(0, size1, fifoBuffer, 0, start2, size2);
    }
    abstractFifo.finishedRead(size1 + size2);

    // round samples to 16bit values
    for (int i = 0; i < bufferSize; ++i)
    {
        float sampleRounded = monoBuffer.getSample(0, i);
        bitBuffer.setSample(0, i, roundBits(sampleRounded));
    }
    // replace input buffer with rounded samples
    bool vintageButtonState = *apvts.getRawParameterValue("VINTAGE");
    if (vintageButtonState == true)
    {
        monoBuffer.copyFrom(0, 0, bitBuffer, 0, 0, bufferSize);
    }
    // generate white noise
    float noiseLevel = 0.00012f;
    float noiseLevelHalf = 0.00006f;
    for (int i = 0; i < bufferSize; ++i)
    {
        float noiseSample = (random.nextFloat() * noiseLevel) - noiseLevelHalf;
        noiseBuffer.setSample(0, i, noiseSample);
    }
    // sum input buffer & noise buffer together
    bool noiseButtonState = *apvts.getRawParameterValue("NOISE");
    if (noiseButtonState == true)
    {
        monoBuffer.addFrom(0, 0, noiseBuffer, 0, 0, bufferSize);
    }
    // apply anti-aliasing filter
    antiAliasFirstSection.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    antiAliasSecondSection.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    antiAliasThirdSection.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    // calculate & process the delay taps
    auto *data = monoBuffer.getReadPointer(0);
    for (int i = 0; i < bufferSize; i++)
    {
        fractionalDelay.pushSample(0, data[i]);
        // calculate base address factors
        unsigned int decayTime = 15;
        unsigned int preDelay = 3;
        unsigned int gainBaseAddr = ((decayTime & 0x07) << 5) | (programId << 8) | ((decayTime >> 3) << 12);
        unsigned int delayBaseAddr = ((preDelay & 0x07) << 6) | ((programId & 0x07) << 9) | ((preDelay >> 3) << 12) | ((programId >> 3) << 13);
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
            unsigned int gainModContOut = gainModControlData[gainModContAddress + d] & 0x07;
            unsigned int nGainModEnable = (gainModControlData[gainModContAddress + d] & 0x08) >> 3;
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
            float feedbackGain{};
            if (nGSN == 0)
            {
                feedbackGain = gainCeiling[1 + d] * -0.00390625f;
            }
            else
            {
                feedbackGain = gainCeiling[1 + d] * 0.00390625f;
            }
            long readPosition = delayTaps[1 + d];
            int writeIndex = writeAddressArray[writePosition];
            int readIndex = writeAddressArray[readPosition];
            feedbackDelayTime = static_cast<float>(writeIndex - readIndex);
            if (feedbackDelayTime < 1)
            {
                feedbackDelayTime += 65536;
            }
            decaySmooth.setTargetValue(adjustableDecay);
            float nextDecayValue = decaySmooth.getNextValue();
            feedbackDelayTime *= 0.00003125f;
            feedbackDelayGain = feedbackGain * (feedbackDelayGainMult * nextDecayValue);
            feedbackOutputSample += fractionalDelay.popSample(0, feedbackDelayTime * lastSampleRate, false) * feedbackDelayGain;
        }
        gainAddress = gainBaseAddr + 23;
        delayModAddress = delayBaseAddr + 45;
        delayAddress = delayBaseAddr + 46;
        for (int d = 0; d < 8; d++)
        {
            rowInput = delayData[delayModAddress + d * 2] + nROW;
            columnInput = delayData[delayAddress + d * 2] + nCOLUMN;
            delayTaps[16 + d] = calculateAddress(rowInput, columnInput);

            gainCeiling[16 + d] = (gainData[gainAddress + d] << 1) & 255;
            unsigned int nGSN = gainData[gainAddress + d] >> 7;
            if (nGSN == 0)
            {
                outputGainArray[d] = gainCeiling[16 + d] * -0.00390625f;
            }
            else
            {
                outputGainArray[d] = gainCeiling[16 + d] * 0.00390625f;
            }
            long readPosition = delayTaps[16 + d];
            int writeIndex = writeAddressArray[writePosition];
            int readIndex = writeAddressArray[readPosition];
            feedbackDelayTime = static_cast<float>(writeIndex - readIndex);
            if (feedbackDelayTime < 1)
            {
                feedbackDelayTime += 65536;
            }
            outputDelayArray[d] = feedbackDelayTime * 0.00003125f;
        }
        feedbackBuffer.setSample(0, i, feedbackOutputSample);
        // process random sample
        float randomSample = randomBuffer.getSample(0, i);
        if (randomSample < 0)
        {
            randomSample *= -0.33f;
        }
        // scale randomSample by a certain amount
        float randomSampleMult = 8.0f;
        randomSample *= randomSampleMult;
        // calculate rateLVL value
        unsigned int rateLevel = rngsus(randomSample);
        // mod rate counter
        modClockOut += 1;
        if (modClockOut == modRateCeiling)
        {
            unsigned int modRateCount = rateLevel | (programId << 4);
            modClockOut = static_cast<int>((modRateCountData[modRateCount] & 0xf) * modScale);
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
            delayModBaseAddr = (modCount >> 6) << 5;
        }
        // calculate output taps
        float outputDelayGainMult = 0.5f;
        float leftOutputSample{};
        float rightOutputSample{};
        float outputDelayTime{};
        float outputDelayGain{};
        // left output taps
        predelaySmooth.setTargetValue(adjustablePreDelay);
        float nextPreDelayValue = predelaySmooth.getNextValue();
        for (int d = 0; d < 4; d++)
        {
            //outputDelayTime = ((programId * outputDelayArray[d]) + outputDelayArray[d + 8] + nextPreDelayValue) * 0.001f;
            outputDelayTime = outputDelayArray[d] + (nextPreDelayValue * 0.001f);
            outputDelayGain = outputGainArray[d] * outputDelayGainMult;
            leftOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, false) * outputDelayGain;
        }
        // right output taps
        for (int d = 4; d < 7; d++)
        {
            //outputDelayTime = ((programId * outputDelayArray[d]) + outputDelayArray[d + 8] + nextPreDelayValue) * 0.001f;
            outputDelayTime = outputDelayArray[d] + (nextPreDelayValue * 0.001f);
            outputDelayGain = outputGainArray[d] * outputDelayGainMult;
            rightOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, false) * outputDelayGain;
        }
        //outputDelayTime = ((programId * outputDelayArray[7]) + outputDelayArray[7 + 8] + nextPreDelayValue) * 0.001f;
        outputDelayTime = outputDelayArray[7] + (nextPreDelayValue * 0.001f);
        outputDelayGain = outputGainArray[7] * outputDelayGainMult;
        rightOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, true) * outputDelayGain;
        // add left channel to outputBuffer
        outputBuffer.setSample(0, i, leftOutputSample);
        // add right channel to outputBuffer when running in stereo
        if (totalNumOutputChannels == 2)
        {
            outputBuffer.setSample(1, i, rightOutputSample);
        }
    }
    // pre-process feedback buffer
    juce::dsp::AudioBlock<float> feedbackBlock(feedbackBuffer);
    feedBackHighPass.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
    feedBackLowPass.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
    feedBackDip.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));

    // write feedbackbuffer to fifo
    abstractFifo.prepareToWrite(bufferSize, start1, size1, start2, size2);
    if (size1 > 0)
    {
        fifoBuffer.copyFrom(0, start1, feedbackBuffer, 0, 0, size1);
    }
    if (size2 > 0)
    {
        fifoBuffer.copyFrom(0, start2, feedbackBuffer, 0, size1, size2);
    }
    abstractFifo.finishedWrite(size1 + size2);

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
        mixSmooth.setTargetValue(mixLevel);
        float wetLevel = mixSmooth.getNextValue();
        float dryLevel = 1.0f - wetLevel;
        buffer.copyFromWithRamp(channel, 0, drySignal, bufferSize, dryLevel, dryLevel);
        buffer.addFromWithRamp(channel, 0, wetSignal, bufferSize, wetLevel, wetLevel);
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
                                                                      juce::StringArray("Plate 1", "Plate 2", "Chamber", "Small Hall", "Hall", "Large Hall", "Cathedral", "Canyon"), 3));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("PREDELAY", "Pre Delay", 0.0f, 320.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", 0.0f, 100.0f, 70.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.0f, 100.0f, 50.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFDECAY", "Low Frequency Decay", 20.0f, 480.0f, 20.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("HFDECAY", "High Frequency Decay", 3000.0f, 16000.0f, 16000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("INPUT", "Input Gain", 0.0f, 2.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("VINTAGE", "Vintage", true));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("NOISE", "Noise", true));
    return {parameters.begin(), parameters.end()};
}