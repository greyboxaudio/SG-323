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
    inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    inputGainSlider.setTextBoxIsEditable(false);
    inputGainSlider.setLookAndFeel (&blueKnob);
    addAndMakeVisible(inputGainSlider);
    inputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT", inputGainSlider);
    addAndMakeVisible(inputGainLabel);
    inputGainLabel.setText("input", juce::dontSendNotification);
    inputGainLabel.attachToComponent(&inputGainSlider, false);
    inputGainLabel.setJustificationType(juce::Justification::centredTop);

    highPassSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    highPassSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    highPassSlider.setTextBoxIsEditable(false);
    highPassSlider.setLookAndFeel (&whiteKnob);
    addAndMakeVisible(highPassSlider);
    highPassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HPF", highPassSlider);
    addAndMakeVisible(highPassLabel);
    highPassLabel.setText("LF decay", juce::dontSendNotification);
    highPassLabel.attachToComponent(&highPassSlider, false);
    highPassLabel.setJustificationType(juce::Justification::centredTop);

    lowPassSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lowPassSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    lowPassSlider.setTextBoxIsEditable(false);
    lowPassSlider.setLookAndFeel (&whiteKnob);
    addAndMakeVisible(lowPassSlider);
    lowPassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LPF", lowPassSlider);
    addAndMakeVisible(lowPassLabel);
    lowPassLabel.setText("HF decay", juce::dontSendNotification);
    lowPassLabel.attachToComponent(&lowPassSlider, false);
    lowPassLabel.setJustificationType(juce::Justification::centredTop);

    wetDrySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    wetDrySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    wetDrySlider.setTextBoxIsEditable(false);
    wetDrySlider.setLookAndFeel (&blueKnob);
    addAndMakeVisible(wetDrySlider);
    wetDrySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WETDRY", wetDrySlider);
    addAndMakeVisible(wetDryLabel);
    wetDryLabel.setText("mix", juce::dontSendNotification);
    wetDryLabel.attachToComponent(&wetDrySlider, false);
    wetDryLabel.setJustificationType(juce::Justification::centredTop);

    predelaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    predelaySlider.setTextBoxIsEditable(false);
    predelaySlider.setLookAndFeel (&redKnob);
    addAndMakeVisible(predelaySlider);
    predelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PREDELAY", predelaySlider);
    addAndMakeVisible(predelayLabel);
    predelayLabel.setText("pre delay", juce::dontSendNotification);
    predelayLabel.attachToComponent(&predelaySlider, false);
    predelayLabel.setJustificationType(juce::Justification::centredTop);

    decaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    decaySlider.setTextBoxIsEditable(false);
    decaySlider.setLookAndFeel (&redKnob);
    addAndMakeVisible(decaySlider);
    decaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DECAY", decaySlider);
    addAndMakeVisible(decayLabel);
    decayLabel.setText("decay time", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, false);
    decayLabel.setJustificationType(juce::Justification::centred);

    //addAndMakeVisible(svg);

    noiseButton.setButtonText("Noise");
    //addAndMakeVisible(noiseButton);
    noiseButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "NOISE", noiseButton);

    bitReduceButton.setButtonText("16bit");
    //addAndMakeVisible(bitReduceButton);
    bitReduceButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BITREDUCE", bitReduceButton);

    programBox.addItem("Plate 1", 1);
    programBox.addItem("Plate 2", 2);
    programBox.addItem("Chamber", 3);
    programBox.addItem("Small Hall", 4);
    programBox.addItem("Hall", 5);
    programBox.addItem("Large Hall", 6);
    programBox.addItem("Cathedral", 7);
    programBox.addItem("Canyon", 8);
    programBox.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(programBox);
    programBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "PROGRAM", programBox);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(720, 240);
}

SG323AudioProcessorEditor::~SG323AudioProcessorEditor()
{
}

//==============================================================================
void SG323AudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour (89,89,89));
    
    auto graphicsArea = getBounds();
    auto graphicsAreaWidth = graphicsArea.getWidth();
    auto graphicsAreaHeight = graphicsArea.getHeight();

    g.setColour(juce::Colours::white);
    g.setFont(14.0f * (graphicsAreaHeight * 0.00416667f));
    g.drawFittedText ("v0.7.0 " __DATE__ " " __TIME__, getLocalBounds(), juce::Justification::topRight, 1);

    g.setColour (juce::Colours::orange);
    juce::Rectangle<int> imageArea (juce::Point<int> (0, 0),juce::Point<int> (static_cast<int>(graphicsAreaWidth * 0.16666667f), static_cast<int>(graphicsAreaHeight * 0.4f)));
    juce::Rectangle<int> textArea (juce::Point<int> (static_cast<int>(graphicsAreaWidth * 0.16666667f), 0),juce::Point<int> (static_cast<int>(graphicsAreaWidth * 0.66666667f), static_cast<int>(graphicsAreaHeight * 0.4f)));
    juce::Rectangle<int> boxArea (juce::Point<int> (static_cast<int>(graphicsAreaWidth * 0.16666667f), 0),juce::Point<int> (static_cast<int>(graphicsAreaWidth * 1.0f), static_cast<int>(graphicsAreaHeight * 0.4f)));
    g.drawRect (imageArea,2);
    g.drawRect (textArea,2);
    g.drawRect (boxArea,2);
    /*
    companyLogo = juce::ImageCache::getFromMemory(BinaryData::greyboxaudiocatbw_png,BinaryData::greyboxaudiocatbw_pngSize);
    g.drawImageWithin(companyLogo,0,0,static_cast<int>(graphicsAreaWidth * 0.166666666f),static_cast<int>(graphicsAreaHeight * 0.4),1,false);

    g.drawText("LEO MINOR",graphicsArea.removeFromLeft(graphicsAreaWidth * 0.333333333333f),Justification::left);
    */
}

void SG323AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    setResizable (true, true);
    setResizeLimits(720,240,1440,480);
    const float ratio = 3.0f;
	  getConstrainer ()->setFixedAspectRatio (ratio);

    auto area = getLocalBounds();
    auto areaHeight = area.getHeight() * 0.5f;
    auto areaWidth = area.getWidth() * 0.16666667f;
    auto areaBottom = area.removeFromBottom(area.getHeight() * 0.5f);
    //noiseButton.setBounds(25,25,100,50);
    //bitReduceButton.setBounds(100,25,100,50);
    programBox.setBounds(area.removeFromRight(area.getWidth() * 0.33333334f));
    //svg.setBounds(400, 25, 150, 50);
    
    inputGainSlider.setBounds(areaBottom.removeFromLeft(areaWidth));
    highPassSlider.setBounds(areaBottom.removeFromLeft(areaWidth));
    lowPassSlider.setBounds(areaBottom.removeFromLeft(areaWidth));
    wetDrySlider.setBounds(areaBottom.removeFromLeft(areaWidth));
    predelaySlider.setBounds(areaBottom.removeFromLeft(areaWidth));
    decaySlider.setBounds(areaBottom.removeFromLeft(areaWidth));
}