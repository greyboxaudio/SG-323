/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "style.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SG323AudioProcessorEditor : public juce::AudioProcessorEditor,
                                  public juce::Button::Listener
{
public:
    SG323AudioProcessorEditor(SG323AudioProcessor&);
    ~SG323AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void buttonClicked (Button*) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    float menuBarHeight = 0.1f;
    float editorScale = 1.0f;
    int defaultHeight = 280;
    int defaultWidth = 720;
    
    juce::Image companyLogo;
    BlueKnob blueKnob;
    RedKnob redKnob;
    WhiteKnob whiteKnob;
    RedBox redBox;

    juce::ComboBox programBox;
    juce::Slider predelaySlider;
    juce::Slider decaySlider;
    juce::Slider wetDrySlider;
    juce::Slider highPassSlider;
    juce::Slider lowPassSlider;
    juce::Slider inputGainSlider;
    juce::ToggleButton noiseButton;
    juce::ToggleButton bitReduceButton;
    juce::TextButton button;

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
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> noiseButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bitReduceButtonAttachment;
    SG323AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SG323AudioProcessorEditor)
};
