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
/***/

class SG323AudioProcessorEditor : public juce::AudioProcessorEditor,
                                  public juce::Button::Listener
{
public:
  SG323AudioProcessorEditor(SG323AudioProcessor &);
  ~SG323AudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  void buttonClicked(juce::Button* button) override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  float menuBarHeight = 0.1f;
  double editorScale = 1.0;
  int defaultHeight = 280;
  int defaultWidth = 720;
  float fontSizeLarge = 24.0f;
  float fontSizeRegular = 16.0f;

  juce::Colour backgroundColour = juce::Colour(70, 74, 70);
  juce::Colour headerColour = juce::Colour(25, 25, 25);
  juce::Colour textColour = juce::Colour(255, 255, 255);

  juce::Image companyLogo;
  CustomTextButton customTextButton;
  CustomToggleButton customToggleButton;
  BlueKnob blueKnob;
  RedKnob redKnob;
  WhiteKnob whiteKnob;
  RedBox redBox;
  CustomKnobLabel customKnobLabel;

  juce::ComboBox programBox;
  juce::Slider predelaySlider;
  juce::Slider decaySlider;
  juce::Slider mixSlider;
  juce::Slider lfdecaySlider;
  juce::Slider hfdecaySlider;
  juce::Slider inputGainSlider;
  juce::TextButton resizeButton;
  juce::ToggleButton vintageButton;
  juce::ToggleButton noiseButton;
  //juce::ToggleButton mixLockButton;
  juce::HyperlinkButton urlButton;

  juce::Label predelayLabel;
  juce::Label decayLabel;
  juce::Label mixLabel;
  juce::Label lfdecayLabel;
  juce::Label hfdecayLabel;
  juce::Label inputGainLabel;

  ApplicationProperties applicationProperties;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> programBoxAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decaySliderAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> predelaySliderAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfdecaySliderAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hfdecaySliderAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> vintageButtonAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> noiseButtonAttachment;
  SG323AudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SG323AudioProcessorEditor)
};
