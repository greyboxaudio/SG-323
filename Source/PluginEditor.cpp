/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SG323AudioProcessorEditor::SG323AudioProcessorEditor(SG323AudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

  pluginLabel.setText("SG-323", juce::dontSendNotification);
  // pluginLabel.setFont(juce::Font (16.0f * (getHeight()/800), juce::Font::bold));
  addAndMakeVisible(pluginLabel);
  versionLabel.setText("v0.6.1", juce::dontSendNotification);
  addAndMakeVisible(versionLabel);

  inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
  inputGainSlider.setTextBoxIsEditable(false);
  addAndMakeVisible(inputGainSlider);
  inputGainLabel.setText("INPUT", juce::dontSendNotification);
  inputGainLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(inputGainLabel);
  inputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT", inputGainSlider);

  highPassSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
  highPassSlider.setTextBoxIsEditable(false);
  addAndMakeVisible(highPassSlider);
  highPassLabel.setText("HPF", juce::dontSendNotification);
  highPassLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(highPassLabel);
  highPassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HPF", highPassSlider);

  lowPassSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
  lowPassSlider.setTextBoxIsEditable(false);
  addAndMakeVisible(lowPassSlider);
  lowPassLabel.setText("LPF", juce::dontSendNotification);
  lowPassLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(lowPassLabel);
  lowPassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LPF", lowPassSlider);

  predelaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
  predelaySlider.setTextBoxIsEditable(false);
  addAndMakeVisible(predelaySlider);
  predelayLabel.setText("PREDLY", juce::dontSendNotification);
  predelayLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(predelayLabel);
  predelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PREDELAY", predelaySlider);

  decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
  decaySlider.setTextBoxIsEditable(false);
  addAndMakeVisible(decaySlider);
  decayLabel.setText("DECAY", juce::dontSendNotification);
  decayLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(decayLabel);
  decaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DECAY", decaySlider);

  wetDrySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
  wetDrySlider.setTextBoxIsEditable(false);
  addAndMakeVisible(wetDrySlider);
  wetDryLabel.setText("MIX", juce::dontSendNotification);
  wetDryLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(wetDryLabel);
  wetDrySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WETDRY", wetDrySlider);

  debugSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
  debugSlider.setTextBoxIsEditable(false);
  // addAndMakeVisible(debugSlider);
  debugSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DEBUG", debugSlider);

  noiseButton.setButtonText("Noise");
  addAndMakeVisible(noiseButton);
  noiseButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "NOISE", noiseButton);

  bitReduceButton.setButtonText("16bit");
  addAndMakeVisible(bitReduceButton);
  bitReduceButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BITREDUCE", bitReduceButton);

  testButton.setButtonText("Test");
  addAndMakeVisible(testButton);
  testButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "TEST", testButton);

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
  setResizable(1, 1);
  setSize(800, 600);
}

SG323AudioProcessorEditor::~SG323AudioProcessorEditor()
{
}

//==============================================================================
void SG323AudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
}

void SG323AudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  juce::Rectangle<int> bounds = getLocalBounds();
  // top flexbox
  juce::FlexBox flexboxTop;
  flexboxTop.flexDirection = juce::FlexBox::Direction::row;
  flexboxTop.flexWrap = juce::FlexBox::Wrap::noWrap;
  flexboxTop.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
  flexboxTop.alignContent = juce::FlexBox::AlignContent::center;
  juce::Array<FlexItem> itemArrayTop;
  itemArrayTop.add(FlexItem(pluginLabel).withMargin(8.0f).withFlex(1));
  itemArrayTop.add(FlexItem(programBox).withMinWidth(90.0f).withMinHeight(30.0f).withMaxWidth(180.0f).withMaxHeight(60.0f).withMargin(8.0f).withFlex(1));
  flexboxTop.items = itemArrayTop;
  flexboxTop.performLayout(bounds.toFloat());
  // center flexbox
  juce::FlexBox flexboxLabels;
  flexboxLabels.flexDirection = juce::FlexBox::Direction::row;
  flexboxLabels.flexWrap = juce::FlexBox::Wrap::noWrap;
  flexboxLabels.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
  flexboxLabels.alignContent = juce::FlexBox::AlignContent::spaceBetween;
  juce::Array<FlexItem> itemArrayLabels;
  itemArrayLabels.add(FlexItem(inputGainLabel).withFlex(1));
  itemArrayLabels.add(FlexItem(highPassLabel).withFlex(1));
  itemArrayLabels.add(FlexItem(lowPassLabel).withFlex(1));
  itemArrayLabels.add(FlexItem(predelayLabel).withFlex(1));
  itemArrayLabels.add(FlexItem(decayLabel).withFlex(1));
  itemArrayLabels.add(FlexItem(wetDryLabel).withFlex(1));
  flexboxLabels.items = itemArrayLabels;
  flexboxLabels.performLayout(bounds.toFloat());
  // center flexbox
  juce::FlexBox flexboxCenter;
  flexboxCenter.flexDirection = juce::FlexBox::Direction::row;
  flexboxCenter.flexWrap = juce::FlexBox::Wrap::noWrap;
  flexboxCenter.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
  flexboxCenter.alignContent = juce::FlexBox::AlignContent::spaceBetween;
  juce::Array<FlexItem> itemArrayCenter;
  itemArrayCenter.add(FlexItem(inputGainSlider).withMinWidth(80.0f).withMinHeight(120.0f).withMargin(8.0f).withFlex(1));
  itemArrayCenter.add(FlexItem(highPassSlider).withMinWidth(80.0f).withMinHeight(120.0f).withMargin(8.0f).withFlex(1));
  itemArrayCenter.add(FlexItem(lowPassSlider).withMinWidth(80.0f).withMinHeight(120.0f).withMargin(8.0f).withFlex(1));
  itemArrayCenter.add(FlexItem(predelaySlider).withMinWidth(80.0f).withMinHeight(120.0f).withMargin(8.0f).withFlex(1));
  itemArrayCenter.add(FlexItem(decaySlider).withMinWidth(80.0f).withMinHeight(120.0f).withMargin(8.0f).withFlex(1));
  itemArrayCenter.add(FlexItem(wetDrySlider).withMinWidth(80.0f).withMinHeight(120.0f).withMargin(8.0f).withFlex(1));
  // debugSlider.setBounds(475, 120, 50, 200);
  flexboxCenter.items = itemArrayCenter;
  flexboxCenter.performLayout(bounds.toFloat());
  // bottom flexbox
  juce::FlexBox flexboxBottom;
  flexboxBottom.flexDirection = juce::FlexBox::Direction::row;
  flexboxBottom.flexWrap = juce::FlexBox::Wrap::noWrap;
  flexboxBottom.justifyContent = juce::FlexBox::JustifyContent::flexStart;
  flexboxBottom.alignContent = juce::FlexBox::AlignContent::center;
  juce::Array<FlexItem> itemArrayBottom;
  itemArrayBottom.add(FlexItem(noiseButton).withMinWidth(120.0f).withMinHeight(30.0f).withMargin(8.0f).withFlex(1));
  itemArrayBottom.add(FlexItem(bitReduceButton).withMinWidth(120.0f).withMinHeight(30.0f).withMargin(8.0f).withFlex(1));
  itemArrayBottom.add(FlexItem(testButton).withMinWidth(120.0f).withMinHeight(30.0f).withMargin(8.0f).withFlex(1));
  itemArrayBottom.add(FlexItem(versionLabel).withMargin(8.0f).withFlex(1));
  flexboxBottom.items = itemArrayBottom;
  flexboxBottom.performLayout(bounds.toFloat());
  // main flexbox
  juce::FlexBox flexboxMain;
  flexboxMain.flexDirection = juce::FlexBox::Direction::column;
  juce::Array<FlexItem> itemArrayMain;
  itemArrayMain.add(FlexItem(flexboxTop).withMaxHeight(getHeight() * 0.2f).withFlex(1));
  itemArrayMain.add(FlexItem(flexboxLabels).withMaxHeight(getHeight() * 0.1f).withFlex(1));
  itemArrayMain.add(FlexItem(flexboxCenter).withFlex(1));
  itemArrayMain.add(FlexItem(flexboxBottom).withMaxHeight(getHeight() * 0.1f).withFlex(1));
  flexboxMain.items = itemArrayMain;
  flexboxMain.performLayout(getLocalBounds().toFloat());
}
