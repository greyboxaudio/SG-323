/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SG323AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SG323AudioProcessorEditor(SG323AudioProcessor&);
    ~SG323AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::ComboBox programBox;
    juce::Slider predelaySlider;
    juce::Slider decaySlider;
    juce::Slider wetDrySlider;
    juce::Slider highPassSlider;
    juce::Slider lowPassSlider;
    juce::Slider inputGainSlider;
    juce::Slider feedbackSlider;

    juce::Label predelayLabel;
    juce::Label decayLabel;
    juce::Label wetDryLabel;
    juce::Label highPassLabel;
    juce::Label lowPassLabel;
    juce::Label inputGainLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> programBoxAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decaySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> predelaySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetDrySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttachment;
    SG323AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SG323AudioProcessorEditor)
};
