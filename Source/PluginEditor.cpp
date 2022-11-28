/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SG323AudioProcessorEditor::SG323AudioProcessorEditor(SG323AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    inputGainSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(inputGainSlider);
    inputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT", inputGainSlider);
    addAndMakeVisible(inputGainLabel);
    inputGainLabel.setText("INPUT", juce::dontSendNotification);
    inputGainLabel.attachToComponent(&inputGainSlider, false);
    inputGainLabel.setJustificationType(juce::Justification::centred);

    highPassSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    highPassSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(highPassSlider);
    highPassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HPF", highPassSlider);
    addAndMakeVisible(highPassLabel);
    highPassLabel.setText("HPF", juce::dontSendNotification);
    highPassLabel.attachToComponent(&highPassSlider, false);
    highPassLabel.setJustificationType(juce::Justification::centred);

    lowPassSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    lowPassSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(lowPassSlider);
    lowPassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LPF", lowPassSlider);
    addAndMakeVisible(lowPassLabel);
    lowPassLabel.setText("LPF", juce::dontSendNotification);
    lowPassLabel.attachToComponent(&lowPassSlider, false);
    lowPassLabel.setJustificationType(juce::Justification::centred);

    predelaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    predelaySlider.setTextBoxIsEditable(false);
    addAndMakeVisible(predelaySlider);
    predelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PREDELAY", predelaySlider);
    addAndMakeVisible(predelayLabel);
    predelayLabel.setText("PREDLY", juce::dontSendNotification);
    predelayLabel.attachToComponent(&predelaySlider, false);
    predelayLabel.setJustificationType(juce::Justification::centred);

    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    decaySlider.setTextBoxIsEditable(false);
    addAndMakeVisible(decaySlider);
    decaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DECAY", decaySlider);
    addAndMakeVisible(decayLabel);
    decayLabel.setText("DECAY", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, false);
    decayLabel.setJustificationType(juce::Justification::centred);

    wetDrySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    wetDrySlider.setTextBoxIsEditable(false);
    addAndMakeVisible(wetDrySlider);
    wetDrySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WETDRY", wetDrySlider);
    addAndMakeVisible(wetDryLabel);
    wetDryLabel.setText("MIX", juce::dontSendNotification);
    wetDryLabel.attachToComponent(&wetDrySlider, false);
    wetDryLabel.setJustificationType(juce::Justification::centred);

    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    feedbackSlider.setTextBoxIsEditable(false);
    addAndMakeVisible(feedbackSlider);
    feedbackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FDBK", feedbackSlider);

    programBox.addItem("Plate 1", 1);
    programBox.addItem("Plate 2", 2);
    programBox.addItem("Chamber", 3);
    programBox.addItem("Small Hall", 4);
    programBox.addItem("Hall", 5);
    programBox.addItem("Large Hall", 6);
    programBox.addItem("Cathedral", 7);
    programBox.addItem("Canyon", 8);
    programBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(programBox);
    programBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "PROGRAM", programBox);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(550, 350);
}

SG323AudioProcessorEditor::~SG323AudioProcessorEditor()
{
}

//==============================================================================
void SG323AudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText ("v0.4.1", getLocalBounds(), juce::Justification::bottomRight, 1);
}

void SG323AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::Rectangle<int> bounds = getLocalBounds();
    programBox.setBounds(300, 25, 150, 50);
    inputGainSlider.setBounds(25, 120, 50, 200);
    highPassSlider.setBounds(100, 120, 50, 200);
    lowPassSlider.setBounds(175, 120, 50, 200);
    predelaySlider.setBounds(250, 120, 50, 200);
    decaySlider.setBounds(325, 120, 50, 200);
    wetDrySlider.setBounds(400, 120, 50, 200);
    feedbackSlider.setBounds(475, 120, 50, 200);
}
