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
  juce::PropertiesFile::Options options;
  options.applicationName = ProjectInfo::projectName;
  options.commonToAllUsers = true;
  options.filenameSuffix = "settings";
  options.osxLibrarySubFolder = "Application Support";
  applicationProperties.setStorageParameters(options);
  if (auto *properties = applicationProperties.getCommonSettings(true))
  {
    editorScale = properties->getDoubleValue("scalingFactor", 1.0);
  }

  if (editorScale == 1.0)
  {
    resizeButton.setButtonText("100%");
  }
  else
  {
    resizeButton.setToggleState(true, true);
    resizeButton.setButtonText("150%");
  }
  resizeButton.addListener(this);
  resizeButton.setLookAndFeel(&customTextButton);
  customTextButton.setFontSize(fontSizeRegular * editorScale);
  addAndMakeVisible(resizeButton);
  resizeButton.setClickingTogglesState(true);

  customToggleButton.setFontSize(fontSizeRegular * editorScale);

  vintageButton.setButtonText("Vintage");
  vintageButton.setLookAndFeel(&customToggleButton);
  addAndMakeVisible(vintageButton);
  vintageButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "VINTAGE", vintageButton);

  noiseButton.setButtonText("Noise");
  noiseButton.setLookAndFeel(&customToggleButton);
  addAndMakeVisible(noiseButton);
  noiseButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "NOISE", noiseButton);

  /*mixLockButton.setButtonText("Mix Lock");
  mixLockButton.setLookAndFeel(&customToggleButton);
  addAndMakeVisible(mixLockButton);*/

  programBox.addItem("Plate 1", 1);
  programBox.addItem("Plate 2", 2);
  programBox.addItem("Chamber", 3);
  programBox.addItem("Small Hall", 4);
  programBox.addItem("Hall", 5);
  programBox.addItem("Large Hall", 6);
  programBox.addItem("Cathedral", 7);
  programBox.addItem("Canyon", 8);
  programBox.setLookAndFeel(&redBox);
  addAndMakeVisible(programBox);
  redBox.setFontSize(fontSizeLarge * editorScale);
  programBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "PROGRAM", programBox);

  customKnobLabel.setFontSize(fontSizeRegular * editorScale);

  inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
  inputGainSlider.setTextBoxIsEditable(true);
  inputGainSlider.setLookAndFeel(&blueKnob);
  addAndMakeVisible(inputGainSlider);
  inputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT", inputGainSlider);
  addAndMakeVisible(inputGainLabel);
  inputGainLabel.setText("input", juce::dontSendNotification);
  inputGainLabel.attachToComponent(&inputGainSlider, false);
  inputGainLabel.setJustificationType(juce::Justification::centredTop);
  inputGainLabel.setLookAndFeel(&customKnobLabel);

  lfdecaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  lfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
  lfdecaySlider.setTextBoxIsEditable(true);
  lfdecaySlider.setLookAndFeel(&whiteKnob);
  addAndMakeVisible(lfdecaySlider);
  lfdecaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LFDECAY", lfdecaySlider);
  addAndMakeVisible(lfdecayLabel);
  lfdecayLabel.setText("LF decay", juce::dontSendNotification);
  lfdecayLabel.attachToComponent(&lfdecaySlider, false);
  lfdecayLabel.setJustificationType(juce::Justification::centredTop);
  lfdecayLabel.setLookAndFeel(&customKnobLabel);

  hfdecaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  hfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
  hfdecaySlider.setTextBoxIsEditable(true);
  hfdecaySlider.setLookAndFeel(&whiteKnob);
  addAndMakeVisible(hfdecaySlider);
  hfdecaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HFDECAY", hfdecaySlider);
  addAndMakeVisible(hfdecayLabel);
  hfdecayLabel.setText("HF decay", juce::dontSendNotification);
  hfdecayLabel.attachToComponent(&hfdecaySlider, false);
  hfdecayLabel.setJustificationType(juce::Justification::centredTop);
  hfdecayLabel.setLookAndFeel(&customKnobLabel);

  mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
  mixSlider.setTextBoxIsEditable(true);
  mixSlider.setLookAndFeel(&blueKnob);
  addAndMakeVisible(mixSlider);
  mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIX", mixSlider);
  addAndMakeVisible(mixLabel);
  mixLabel.setText("mix", juce::dontSendNotification);
  mixLabel.attachToComponent(&mixSlider, false);
  mixLabel.setJustificationType(juce::Justification::centredTop);
  mixLabel.setLookAndFeel(&customKnobLabel);

  predelaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
  predelaySlider.setTextBoxIsEditable(true);
  predelaySlider.setLookAndFeel(&redKnob);
  addAndMakeVisible(predelaySlider);
  predelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PREDELAY", predelaySlider);
  addAndMakeVisible(predelayLabel);
  predelayLabel.setText("pre delay", juce::dontSendNotification);
  predelayLabel.attachToComponent(&predelaySlider, false);
  predelayLabel.setJustificationType(juce::Justification::centredTop);
  predelayLabel.setLookAndFeel(&customKnobLabel);

  decaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
  decaySlider.setTextBoxIsEditable(true);
  decaySlider.setLookAndFeel(&redKnob);
  addAndMakeVisible(decaySlider);
  decaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DECAY", decaySlider);
  addAndMakeVisible(decayLabel);
  decayLabel.setText("decay time", juce::dontSendNotification);
  decayLabel.attachToComponent(&decaySlider, false);
  decayLabel.setJustificationType(juce::Justification::centred);
  decayLabel.setLookAndFeel(&customKnobLabel);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(static_cast<int>(defaultWidth * editorScale), static_cast<int>(defaultHeight * editorScale));
}

SG323AudioProcessorEditor::~SG323AudioProcessorEditor()
{
}

//==============================================================================
void SG323AudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(backgroundColour);

  auto graphicsArea = getLocalBounds();
  juce::Rectangle<int> headerArea(juce::Point<int>(graphicsArea.getX(), graphicsArea.getY()), juce::Point<int>(graphicsArea.getRight(), graphicsArea.getBottom() * menuBarHeight));
  g.setColour(headerColour);
  g.fillRect(headerArea);

  juce::Rectangle<int> footerArea(juce::Point<int>(graphicsArea.getX(), graphicsArea.getBottom()* (1.0f-menuBarHeight)), juce::Point<int>(graphicsArea.getRight(), graphicsArea.getBottom()));
  //g.fillRect(footerArea);
  //graphicsArea.removeFromBottom(graphicsArea.getHeight() * menuBarHeight);
  
  graphicsArea.removeFromTop(graphicsArea.getHeight() * menuBarHeight);
  auto graphicsAreaWidth = graphicsArea.getWidth();
  auto graphicsAreaHeight = graphicsArea.getHeight();
  juce::Rectangle<int> imageArea(juce::Point<int>(graphicsArea.getX(), graphicsArea.getY()), juce::Point<int>(graphicsArea.getRight() * 0.16666667f, graphicsArea.getBottom() * 0.4f));
  juce::Rectangle<int> textArea(juce::Point<int>(graphicsArea.getRight() * 0.16666667f, graphicsArea.getY()), juce::Point<int>(graphicsArea.getRight() * 0.66666667f, graphicsArea.getBottom() * 0.4f));
  juce::Rectangle<int> boxArea (juce::Point<int> (graphicsArea.getRight() * 0.66666667f, graphicsArea.getY()),juce::Point<int> (graphicsArea.getRight() * 1.0f, graphicsArea.getBottom() * 0.4f));

  // draw rectangles for visual debugging
  /*g.setColour(juce::Colours::green);
  g.drawRect (headerArea,2);
  g.setColour (juce::Colours::purple);
  g.drawRect (graphicsArea,4);
  g.setColour (juce::Colours::red);
  g.drawRect (imageArea,2);
  g.setColour (juce::Colours::orange);
  g.drawRect (textArea,2);
  g.setColour (juce::Colours::yellow);
  g.drawRect (boxArea,2);
  g.setColour (juce::Colours::blue);
  g.drawRect (footerArea,2);*/

  g.setColour(juce::Colours::white);
  g.setFont(fontSizeRegular * editorScale);
  //g.drawFittedText("v0.8.0 " __DATE__ " " __TIME__, headerArea, juce::Justification::centredRight, 1);
  g.drawFittedText ("www.greyboxaudio.com | v1.0.0 ", headerArea, juce::Justification::centredRight, 1);
  g.setFont(fontSizeLarge * editorScale);
  companyLogo = juce::ImageCache::getFromMemory(BinaryData::greyboxaudiocatbw_png, BinaryData::greyboxaudiocatbw_pngSize);
  g.drawImageWithin(companyLogo, imageArea.getX(), imageArea.getY(), imageArea.getWidth(), imageArea.getHeight(), 36, false);
  g.drawText("LEO MINOR", textArea.removeFromTop(textArea.getHeight() * 0.5f), Justification::bottomLeft);
  g.drawText("SG-323 DIGITAL REVERBERATOR", textArea, Justification::topLeft);
}

void SG323AudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  setResizable(false, false);
  if (auto *properties = applicationProperties.getCommonSettings(true))
  {
    properties->setValue("scalingFactor", editorScale);
  }

  auto boxAreaMain = getLocalBounds();
  auto boxAreaMainWidth = boxAreaMain.getWidth();
  auto boxAreaMainHeight = boxAreaMain.getHeight();

  resizeButton.setBounds(0, 0, boxAreaMainHeight * menuBarHeight * 3, boxAreaMainHeight * menuBarHeight);
  vintageButton.setBounds(resizeButton.getWidth(), 0, boxAreaMainHeight * menuBarHeight * 3, boxAreaMainHeight * menuBarHeight);
  noiseButton.setBounds(vintageButton.getWidth() + resizeButton.getWidth(), 0, boxAreaMainHeight * menuBarHeight * 3, boxAreaMainHeight * menuBarHeight);
  //mixLockButton.setBounds(noiseButton.getWidth() + vintageButton.getWidth() + resizeButton.getWidth(), 0, boxAreaMainHeight * menuBarHeight * 3, boxAreaMainHeight * menuBarHeight);
  boxAreaMain.removeFromTop(boxAreaMain.getHeight() * menuBarHeight);
  juce::Rectangle<int> boxArea(juce::Point<int>(boxAreaMain.getRight() * 0.70833333f, boxAreaMain.getY() + boxAreaMain.getHeight() * 0.08333333f), juce::Point<int>(boxAreaMain.getRight() * 0.95833333f, boxAreaMain.getY() + boxAreaMain.getHeight() * 0.25f));

  auto area = getLocalBounds();
  auto areaHeight = area.getHeight() * 0.5f;
  auto areaWidth = area.getWidth() * 0.16666667f;
  auto areaBottom = area.removeFromBottom(areaHeight);

  programBox.setBounds(boxArea);
  inputGainSlider.setBounds(areaBottom.removeFromLeft(areaWidth));
  lfdecaySlider.setBounds(areaBottom.removeFromLeft(areaWidth));
  hfdecaySlider.setBounds(areaBottom.removeFromLeft(areaWidth));
  mixSlider.setBounds(areaBottom.removeFromLeft(areaWidth));
  predelaySlider.setBounds(areaBottom.removeFromLeft(areaWidth));
  decaySlider.setBounds(areaBottom.removeFromLeft(areaWidth));
}

void SG323AudioProcessorEditor::buttonClicked(Button *)
{
  if (resizeButton.getToggleState() == true)
  {
    editorScale = 1.5f;
    redBox.setFontSize(fontSizeLarge * editorScale);
    customTextButton.setFontSize(fontSizeRegular * editorScale);
    customToggleButton.setFontSize(fontSizeRegular * editorScale);
    customKnobLabel.setFontSize(fontSizeRegular * editorScale);
    setSize(static_cast<int>(defaultWidth * editorScale), static_cast<int>(defaultHeight * editorScale));
    resizeButton.setButtonText("150%");
  }
  else
  {
    editorScale = 1.0f;
    redBox.setFontSize(fontSizeLarge * editorScale);
    customTextButton.setFontSize(fontSizeRegular * editorScale);
    customToggleButton.setFontSize(fontSizeRegular * editorScale);
    customKnobLabel.setFontSize(fontSizeRegular * editorScale);
    setSize(static_cast<int>(defaultWidth * editorScale), static_cast<int>(defaultHeight * editorScale));
    resizeButton.setButtonText("100%");
  }
}