/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::String versionNumber = PLUGIN_VERSION;
#ifndef DEVBUILD
juce::String pluginVersionText = versionNumber;
#else
juce::String pluginVersionText = versionNumber + "_dev";
#endif

#ifndef IS_DEMO
juce::String url = "https://store.greyboxaudio.com/releasenotes.html#sg323-" + versionNumber;
juce::String urlButtonText = "greyboxaudio.com";
juce::String bodyText1 = "LEO MINOR";
juce::String bodyText2 = "SG-323 DIGITAL REVERBERATOR";
#else
juce::String url = "https://store.greyboxaudio.com/products/sg-323-reverb";
juce::String urlButtonText = "BUY NOW!";
juce::String bodyText1 = "SG-323 DEMO";
juce::String bodyText2 = "ALL KNOBS ARE DISABLED";
#endif

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
  if (auto *pluginProperties = applicationProperties.getCommonSettings(true))
  {
    editorScale = pluginProperties->getDoubleValue("scalingFactor", 1.0);
  }
  fontSizeRegularScaled = static_cast<float>(fontSizeRegular * editorScale);
  fontSizeLargeScaled = static_cast<float>(fontSizeLarge * editorScale);
  textBoxWidthScaled = static_cast<int>(textBoxWidth * editorScale);
  textBoxHeightScaled = static_cast<int>(textBoxHeight * editorScale);

  if (editorScale == 1.0)
  {
    resizeButton.setButtonText("100%");
  }
  else
  {
    resizeButton.setToggleState(true, dontSendNotification);
    resizeButton.setButtonText("150%");
  }
  resizeButton.addListener(this);
  resizeButton.setLookAndFeel(&customTextButton);
  customTextButton.setFontSize(fontSizeRegularScaled);
  addAndMakeVisible(resizeButton);
  resizeButton.setClickingTogglesState(true);

  reverbClearButton.setButtonText("RVBCLR");
  reverbClearButton.addListener(this);
  reverbClearButton.setLookAndFeel(&customTextButton);
  addAndMakeVisible(reverbClearButton);

  customToggleButton.setFontSize(fontSizeRegularScaled);

  vintageButton.setButtonText("Vintage");
  vintageButton.setLookAndFeel(&customToggleButton);
  addAndMakeVisible(vintageButton);
  vintageButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "VINTAGE", vintageButton);

  noiseButton.setButtonText("Noise");
  noiseButton.setLookAndFeel(&customToggleButton);
  addAndMakeVisible(noiseButton);
  noiseButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "NOISE", noiseButton);

  addAndMakeVisible(urlButton);
  urlButton.setButtonText(urlButtonText);
  juce::URL u(url);
  urlButton.setURL(u);
  urlButton.setJustificationType(juce::Justification::centred);
  urlButton.setColour(juce::HyperlinkButton::textColourId, juce::Colour(255, 255, 255));

  programBox.addSectionHeading("STARGATE 323");
  programBox.addItem("Plate 1", 1);
  programBox.addItem("Plate 2", 2);
  programBox.addItem("Chamber", 3);
  programBox.addItem("Small Hall", 4);
  programBox.addItem("Hall", 5);
  programBox.addItem("Large Hall", 6);
  programBox.addItem("Cathedral", 7);
  programBox.addItem("Canyon", 8);
  programBox.addSeparator();
  programBox.addSectionHeading("STARGATE 626");
  programBox.addItem("Reverb 9", 9);
  programBox.addItem("Reverb A", 10);
  programBox.addItem("Reverb B", 11);
  programBox.addItem("Delay C", 12);
  programBox.addItem("Delay D", 13);
  programBox.addItem("Delay E", 14);
  programBox.addItem("Delay F", 15);
  programBox.addItem("Reverb 0", 16);
  programBox.setLookAndFeel(&redBox);
  addAndMakeVisible(programBox);
  redBox.setFontSize(fontSizeLargeScaled);
  programBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "PROGRAM", programBox);

  customKnobLabel.setFontSize(fontSizeRegularScaled);

  inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
  inputGainSlider.setTextBoxIsEditable(true);
  inputGainSlider.setColour(juce::Slider::textBoxOutlineColourId, backgroundColour);
  inputGainSlider.setColour(juce::Slider::textBoxBackgroundColourId, stargateDark);
  inputGainSlider.setLookAndFeel(&blueKnob);
  addAndMakeVisible(inputGainSlider);
  inputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT", inputGainSlider);
  addAndMakeVisible(inputGainLabel);
  inputGainLabel.setText("input", juce::dontSendNotification);
  inputGainLabel.attachToComponent(&inputGainSlider, false);
  inputGainLabel.setJustificationType(juce::Justification::centred);
  inputGainLabel.setLookAndFeel(&customKnobLabel);

  lfdecaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  lfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
  lfdecaySlider.setTextBoxIsEditable(true);
  lfdecaySlider.setColour(juce::Slider::textBoxOutlineColourId, backgroundColour);
  lfdecaySlider.setColour(juce::Slider::textBoxBackgroundColourId, stargateDark);
  lfdecaySlider.setLookAndFeel(&whiteKnob);
  addAndMakeVisible(lfdecaySlider);
  lfdecaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LFDECAY", lfdecaySlider);
  addAndMakeVisible(lfdecayLabel);
  lfdecayLabel.setText("LF decay", juce::dontSendNotification);
  lfdecayLabel.attachToComponent(&lfdecaySlider, false);
  lfdecayLabel.setJustificationType(juce::Justification::centred);
  lfdecayLabel.setLookAndFeel(&customKnobLabel);

  hfdecaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  hfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
  hfdecaySlider.setTextBoxIsEditable(true);
  hfdecaySlider.setColour(juce::Slider::textBoxOutlineColourId, backgroundColour);
  hfdecaySlider.setColour(juce::Slider::textBoxBackgroundColourId, stargateDark);
  hfdecaySlider.setLookAndFeel(&whiteKnob);
  addAndMakeVisible(hfdecaySlider);
  hfdecaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HFDECAY", hfdecaySlider);
  addAndMakeVisible(hfdecayLabel);
  hfdecayLabel.setText("HF decay", juce::dontSendNotification);
  hfdecayLabel.attachToComponent(&hfdecaySlider, false);
  hfdecayLabel.setJustificationType(juce::Justification::centred);
  hfdecayLabel.setLookAndFeel(&customKnobLabel);

  mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
  mixSlider.setTextBoxIsEditable(true);
  mixSlider.setColour(juce::Slider::textBoxOutlineColourId, backgroundColour);
  mixSlider.setColour(juce::Slider::textBoxBackgroundColourId, stargateDark);
  mixSlider.setLookAndFeel(&blueKnob);
  addAndMakeVisible(mixSlider);
  mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIX", mixSlider);
  addAndMakeVisible(mixLabel);
  mixLabel.setText("mix", juce::dontSendNotification);
  mixLabel.attachToComponent(&mixSlider, false);
  mixLabel.setJustificationType(juce::Justification::centred);
  mixLabel.setLookAndFeel(&customKnobLabel);

  predelaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
  predelaySlider.setTextBoxIsEditable(true);
  predelaySlider.setColour(juce::Slider::textBoxOutlineColourId, backgroundColour);
  predelaySlider.setColour(juce::Slider::textBoxBackgroundColourId, stargateDark);
  predelaySlider.setLookAndFeel(&redKnob);
  addAndMakeVisible(predelaySlider);
  predelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PREDELAY", predelaySlider);
  addAndMakeVisible(predelayLabel);
  predelayLabel.setText("pre delay", juce::dontSendNotification);
  predelayLabel.attachToComponent(&predelaySlider, false);
  predelayLabel.setJustificationType(juce::Justification::centred);
  predelayLabel.setLookAndFeel(&customKnobLabel);

  decaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
  decaySlider.setTextBoxIsEditable(true);
  decaySlider.setColour(juce::Slider::textBoxOutlineColourId, backgroundColour);
  decaySlider.setColour(juce::Slider::textBoxBackgroundColourId, stargateDark);
  decaySlider.setLookAndFeel(&redKnob);
  addAndMakeVisible(decaySlider);
  decaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DECAY", decaySlider);
  addAndMakeVisible(decayLabel);
  decayLabel.setText("decay time", juce::dontSendNotification);
  decayLabel.attachToComponent(&decaySlider, false);
  decayLabel.setJustificationType(juce::Justification::centred);
  decayLabel.setLookAndFeel(&customKnobLabel);

  blueKnob.setFontSize(fontSizeRegularScaled);
  whiteKnob.setFontSize(fontSizeRegularScaled);
  redKnob.setFontSize(fontSizeRegularScaled);

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
  // paint the UI
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(backgroundColour);
  g.setColour(headerColour);
  g.fillRect(headerArea0);
  // draw text & graphic elements
  g.setColour(juce::Colours::white);
  g.setFont(static_cast<float>(fontSizeRegular * editorScale));
  g.drawFittedText(pluginVersionText, headerArea1, juce::Justification::centredRight, 1);
  companyLogo = juce::ImageCache::getFromMemory(BinaryData::greyboxaudiocat_white_png, BinaryData::greyboxaudiocat_white_pngSize);
  g.drawImageWithin(companyLogo, imageArea0.getX(), imageArea0.getY(), imageArea0.getWidth(), imageArea0.getHeight(), 36, false);
  g.setFont(static_cast<float>(fontSizeLarge * editorScale));
  g.drawText(bodyText1, textArea1, Justification::bottomLeft);
  g.drawText(bodyText2, textArea2, Justification::topLeft);
}

void SG323AudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  #ifndef DEVBUILD
  setResizable(false, false);
  #else
  setResizable(true, true);
  #endif
  if (auto *pluginProperties = applicationProperties.getCommonSettings(true))
  {
    pluginProperties->setValue("scalingFactor", editorScale);
  }
  auto area = getLocalBounds();
  // define size variables
  auto headerFooterHeight = area.getHeight() * 0.1;
  auto headerFooterMargin = headerFooterHeight * 0.1;
  auto pluginTopHeight = area.getHeight() * 0.25;
  auto imageWidth = area.getWidth() / 6;
  auto textAreaWidth = area.getWidth() * 0.55;
  auto sliderWidth = area.getWidth() / 6;
  auto comboBoxAreaMargin = pluginTopHeight * 0.25;
  auto textAreaMargin = area.getHeight() * 0.025;
  auto labelAreaHeight = area.getHeight() * 0.1;
  auto textButtonWidth = area.getWidth() * 0.12;
  auto toggleButtonWidth = area.getWidth() * 0.12;
  auto urlButtonWidth = area.getWidth() * 0.3;
  // define header & footer
  headerArea0 = area.removeFromTop(headerFooterHeight);
  footerArea0 = area.removeFromBottom(headerFooterHeight);
  // add Margin to header & footer
  headerArea1 = headerArea0.reduced(headerFooterMargin);
  // define upper plugin area
  mainArea0 = area.removeFromTop(pluginTopHeight);
  // define image area
  imageArea0 = mainArea0.removeFromLeft(imageWidth);
  // define text areas
  textArea0 = mainArea0.removeFromLeft(textAreaWidth);
  textArea1 = textArea0.reduced(textAreaMargin);
  textArea2 = textArea1.removeFromBottom(textArea1.getHeight()*0.5);
  // define combobox area
  comboBoxArea0 = mainArea0;
  // remove some space for the slider labels
  labelArea0 = area.removeFromTop(labelAreaHeight);
  // define lower plugin area
  mainArea1 = area;
  // place control elements
  // place header elements
  resizeButton.setBounds(headerArea1.removeFromLeft(textButtonWidth));
  resizeButton.changeWidthToFitText();
  vintageButton.setBounds(headerArea1.removeFromLeft(toggleButtonWidth));
  noiseButton.setBounds(headerArea1.removeFromLeft(toggleButtonWidth));
  urlButton.setSize(urlButtonWidth,headerFooterHeight);
  urlButton.changeWidthToFitText();
  urlButton.setBoundsToFit(headerArea0.reduced(headerFooterMargin), 36, true);
  // place combobox
  programBox.setBounds(comboBoxArea0.reduced(comboBoxAreaMargin));
  // place sliders
  inputGainSlider.setBounds(mainArea1.removeFromLeft(sliderWidth));
  lfdecaySlider.setBounds(mainArea1.removeFromLeft(sliderWidth));
  hfdecaySlider.setBounds(mainArea1.removeFromLeft(sliderWidth));
  mixSlider.setBounds(mainArea1.removeFromLeft(sliderWidth));
  predelaySlider.setBounds(mainArea1.removeFromLeft(sliderWidth));
  decaySlider.setBounds(mainArea1.removeFromLeft(sliderWidth));
  // place footer elements
  reverbClearButton.setSize(textButtonWidth, headerFooterHeight);
  reverbClearButton.setBoundsToFit(footerArea0.removeFromRight(sliderWidth).reduced(headerFooterMargin), 36, true);
}

void SG323AudioProcessorEditor::buttonClicked(juce::Button *button)
{
  if (button == &reverbClearButton)
  {
    audioProcessor.clearButtonState = true;
  }
  if (button == &resizeButton)
  {
    if (resizeButton.getToggleState() == true)
    {
      editorScale = 1.5f;
      fontSizeLargeScaled = static_cast<float>(fontSizeLarge * editorScale);
      fontSizeRegularScaled = static_cast<float>(fontSizeRegular * editorScale);
      textBoxWidthScaled = static_cast<int>(textBoxWidth * editorScale);
      textBoxHeightScaled = static_cast<int>(textBoxHeight * editorScale);
      redBox.setFontSize(fontSizeLargeScaled);
      customTextButton.setFontSize(fontSizeRegularScaled);
      customToggleButton.setFontSize(fontSizeRegularScaled);
      customKnobLabel.setFontSize(fontSizeRegularScaled);
      inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      lfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      hfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      blueKnob.setFontSize(fontSizeRegularScaled);
      whiteKnob.setFontSize(fontSizeRegularScaled);
      redKnob.setFontSize(fontSizeRegularScaled);
      setSize(static_cast<int>(defaultWidth * editorScale), static_cast<int>(defaultHeight * editorScale));
      resizeButton.setButtonText("150%");
    }
    else
    {
      editorScale = 1.0f;
      fontSizeLargeScaled = static_cast<float>(fontSizeLarge * editorScale);
      fontSizeRegularScaled = static_cast<float>(fontSizeRegular * editorScale);
      textBoxWidthScaled = static_cast<int>(textBoxWidth * editorScale);
      textBoxHeightScaled = static_cast<int>(textBoxHeight * editorScale);
      redBox.setFontSize(fontSizeLargeScaled);
      customTextButton.setFontSize(fontSizeRegularScaled);
      customToggleButton.setFontSize(fontSizeRegularScaled);
      customKnobLabel.setFontSize(fontSizeRegularScaled);
      inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      lfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      hfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, textBoxWidthScaled, textBoxHeightScaled);
      blueKnob.setFontSize(fontSizeRegularScaled);
      whiteKnob.setFontSize(fontSizeRegularScaled);
      redKnob.setFontSize(fontSizeRegularScaled);
      setSize(static_cast<int>(defaultWidth * editorScale), static_cast<int>(defaultHeight * editorScale));
      resizeButton.setButtonText("100%");
    }
  }
}