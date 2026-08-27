/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::String versionNumber = PLUGIN_VERSION;
#ifndef DEVBUILD
juce::String headerText = versionNumber;
#else
juce::String headerText = versionNumber + "_dev";
#endif

#ifndef IS_DEMO
juce::String url = "https://store.greyboxaudio.com/releasenotes.html#sg323-" + versionNumber;
juce::String urlButtonText = "greyboxaudio.com";
juce::String bodyText1 = "LEO MINOR";
juce::String bodyText2 = "SG-323 DIGITAL REVERBERATOR";
float urlButtonScale[2]{0.7f, 0.25f};
#else
juce::String url = "https://store.greyboxaudio.com/products/sg-323-reverb";
juce::String urlButtonText = "BUY NOW!";
juce::String bodyText1 = "SG-323 DEMO";
juce::String bodyText2 = "ALL KNOBS ARE DISABLED";
float urlButtonScale[2]{0.7f, 0.25f};
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
  customTextButton.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
  addAndMakeVisible(resizeButton);
  resizeButton.setClickingTogglesState(true);

  reverbClearButton.setButtonText("RVBCLR");
  reverbClearButton.addListener(this);
  reverbClearButton.setLookAndFeel(&customTextButton);
  addAndMakeVisible(reverbClearButton);

  customToggleButton.setFontSize(static_cast<float>(fontSizeRegular * editorScale));

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
  redBox.setFontSize(static_cast<float>(fontSizeLarge * editorScale));
  programBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "PROGRAM", programBox);

  customKnobLabel.setFontSize(static_cast<float>(fontSizeRegular * editorScale));

  inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
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
  lfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
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
  hfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
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
  mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
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
  predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
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
  decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
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

  blueKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
  whiteKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
  redKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));

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
  // define UI areas
  auto windowArea = getLocalBounds();
  // define header
   headerArea0 = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), windowArea.getY()), juce::Point<int>(windowArea.getRight(), static_cast<int>(windowArea.getBottom() * headerHeight)));
  int headerAreaPadding = static_cast<int>(headerArea0.getHeight() * 0.1);
  headerArea1 = juce::Rectangle<int>(juce::Point<int>((headerArea0.getX() + headerAreaPadding),headerArea0.getY()+headerAreaPadding),juce::Point<int>(headerArea0.getRight()-headerAreaPadding, headerArea0.getBottom()-headerAreaPadding));
   // define footer
  footerArea0 = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), static_cast<int>(windowArea.getBottom() * (1.0f - footerHeight))), juce::Point<int>(windowArea.getRight(), windowArea.getBottom()));
  int footerAreaPadding = static_cast<int>(footerArea0.getHeight() * 0.1);
  footerArea1 = juce::Rectangle<int>(juce::Point<int>((footerArea0.getX() + footerAreaPadding),footerArea0.getY()+footerAreaPadding),juce::Point<int>(footerArea0.getRight()-footerAreaPadding, footerArea0.getBottom()-footerAreaPadding));
  // define upper plugin section
  pluginAreaTop = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), static_cast<int>(windowArea.getBottom() * headerHeight)), juce::Point<int>(windowArea.getRight(), windowArea.getBottom() * (1.0f - mainSectionBottomHeight - footerHeight)));
  // define image area
  imageArea = juce::Rectangle<int>(juce::Point<int>(pluginAreaTop.getX(), pluginAreaTop.getY()), juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 0.16666667), pluginAreaTop.getBottom()));
  // define text areas
  textArea0 = juce::Rectangle<int>(juce::Point<int>(imageArea.getRight(), pluginAreaTop.getY()), juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 0.66666667), pluginAreaTop.getBottom()));
  int textAreaX = textArea0.getX();
  int textAreaY = textArea0.getY();
  int textAreaRight = textArea0.getRight();
  int textAreaBottom = textArea0.getBottom();
  int textAreaHeight = textArea0.getHeight();
  int textAreaPadding = static_cast<int>(textAreaHeight * 0.1);
  textArea1 = juce::Rectangle<int>(juce::Point<int>(textAreaX + textAreaPadding, textAreaY + textAreaPadding), juce::Point<int>(textAreaRight - textAreaPadding, static_cast<int>(textAreaBottom - (textAreaHeight * 0.5))));
  textArea2 = juce::Rectangle<int>(juce::Point<int>(textAreaX + textAreaPadding, static_cast<int>(textAreaBottom - (textAreaHeight * 0.5))), juce::Point<int>(textAreaRight - textAreaPadding, textAreaBottom - textAreaPadding));
  // define comboBox areas
  comboBoxArea0 = juce::Rectangle<int>(juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 0.66666667), pluginAreaTop.getY()), juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 1.0), pluginAreaTop.getBottom()));
  int comboBoxHeight = comboBoxArea0.getHeight();
  int comboBoxPadding = static_cast<int>(comboBoxHeight * 0.25);
  comboBoxArea1 = juce::Rectangle<int>(juce::Point<int>(comboBoxArea0.getX() + comboBoxPadding, comboBoxArea0.getY() + comboBoxPadding), juce::Point<int>(comboBoxArea0.getRight() - comboBoxPadding, comboBoxArea0.getBottom() - comboBoxPadding));
  // define lower plugin section
  pluginAreaBottom = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), static_cast<int>(windowArea.getBottom() * (headerHeight + mainSectionTopHeight))), juce::Point<int>(windowArea.getRight(), windowArea.getBottom() * (1.0f - footerHeight)));
  int labelHeight = static_cast<int>(windowArea.getHeight()*0.05);
  knobArea = juce::Rectangle<int>(juce::Point<int>(pluginAreaBottom.getX(),pluginAreaBottom.getY()+labelHeight),juce::Point<int>(pluginAreaBottom.getRight(),pluginAreaBottom.getBottom()));
  
  // paint the UI
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(backgroundColour);
  g.setColour(headerColour);
  g.fillRect(headerArea0);
  g.setColour(footerColour);
  g.fillRect(footerArea0);

#ifndef DEVBUILD
#else
  // draw rectangles for visual debugging
  g.setColour(juce::Colours::blue);
  g.drawRect(headerArea0, 2);
  g.drawRect(headerArea1, 2);
  g.setColour(juce::Colours::purple);
  g.drawRect(pluginAreaTop, 2);
  g.setColour(juce::Colours::green);
  g.drawRect(pluginAreaBottom, 2);
  g.drawRect(knobArea, 2);
  g.setColour(juce::Colours::red);
  g.drawRect(imageArea, 2);
  g.setColour(juce::Colours::orange);
  g.drawRect(textArea0, 2);
  g.drawRect(textArea1, 2);
  g.drawRect(textArea2, 2);
  g.setColour(juce::Colours::yellow);
  g.drawRect(comboBoxArea0, 2);
  g.drawRect(comboBoxArea1, 2);
  g.setColour(juce::Colours::blue);
  g.drawRect(footerArea0, 2);
  g.drawRect(footerArea1, 2);
#endif

  //draw text & graphic elements
  g.setColour(juce::Colours::white);
  g.setFont(static_cast<float>(fontSizeRegular * editorScale));
  g.drawFittedText(headerText, footerArea0, juce::Justification::centredRight, 1);
  g.setFont(static_cast<float>(fontSizeLarge * editorScale));
  companyLogo = juce::ImageCache::getFromMemory(BinaryData::greyboxaudiocat_white_png, BinaryData::greyboxaudiocat_white_pngSize);
  g.drawImageWithin(companyLogo, imageArea.getX(), imageArea.getY(), imageArea.getWidth(), imageArea.getHeight(), 36, false);
  g.drawText(bodyText1, textArea1, Justification::bottomLeft);
  g.drawText(bodyText2, textArea2, Justification::topLeft);
}

void SG323AudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  setResizable(false, false);
  if (auto *pluginProperties = applicationProperties.getCommonSettings(true))
  {
    pluginProperties->setValue("scalingFactor", editorScale);
  }
  // define UI areas
  auto windowArea = getLocalBounds();
  // define header
  headerArea0 = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), windowArea.getY()), juce::Point<int>(windowArea.getRight(), static_cast<int>(windowArea.getBottom() * headerHeight)));
  int headerAreaPadding = static_cast<int>(headerArea0.getHeight() * 0.1);
  headerArea1 = juce::Rectangle<int>(juce::Point<int>((headerArea0.getX() + headerAreaPadding),headerArea0.getY()+headerAreaPadding),juce::Point<int>(headerArea0.getRight()-headerAreaPadding, headerArea0.getBottom()-headerAreaPadding));
  // define footer
  footerArea0 = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), static_cast<int>(windowArea.getBottom() * (1.0 - footerHeight))), juce::Point<int>(windowArea.getRight(), windowArea.getBottom()));
  // define upper plugin section
  pluginAreaTop = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), static_cast<int>(windowArea.getBottom() * headerHeight)), juce::Point<int>(windowArea.getRight(), windowArea.getBottom() * (1.0 - mainSectionBottomHeight - footerHeight)));
  // define image area
  imageArea = juce::Rectangle<int>(juce::Point<int>(pluginAreaTop.getX(), pluginAreaTop.getY()), juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 0.16666667), pluginAreaTop.getBottom()));
  // define text areas
  textArea0 = juce::Rectangle<int>(juce::Point<int>(imageArea.getRight(), pluginAreaTop.getY()), juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 0.66666667), pluginAreaTop.getBottom()));
  int textAreaX = textArea0.getX();
  int textAreaY = textArea0.getY();
  int textAreaRight = textArea0.getRight();
  int textAreaBottom = textArea0.getBottom();
  int textAreaHeight = textArea0.getHeight();
  int textAreaPadding = static_cast<int>(textAreaHeight * 0.1);
  textArea1 = juce::Rectangle<int>(juce::Point<int>(textAreaX + textAreaPadding, textAreaY + textAreaPadding), juce::Point<int>(textAreaRight - textAreaPadding, static_cast<int>(textAreaBottom - (textAreaHeight * 0.5))));
  textArea2 = juce::Rectangle<int>(juce::Point<int>(textAreaX + textAreaPadding, static_cast<int>(textAreaBottom - (textAreaHeight * 0.5))), juce::Point<int>(textAreaRight - textAreaPadding, textAreaBottom - textAreaPadding));
  // define comboBox areas
  comboBoxArea0 = juce::Rectangle<int>(juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 0.66666667), pluginAreaTop.getY()), juce::Point<int>(static_cast<int>(pluginAreaTop.getRight() * 1.0), pluginAreaTop.getBottom()));
  int comboBoxHeight = comboBoxArea0.getHeight();
  int comboBoxPadding = static_cast<int>(comboBoxHeight * 0.25);
  comboBoxArea1 = juce::Rectangle<int>(juce::Point<int>(comboBoxArea0.getX() + comboBoxPadding, comboBoxArea0.getY() + comboBoxPadding), juce::Point<int>(comboBoxArea0.getRight() - comboBoxPadding, comboBoxArea0.getBottom() - comboBoxPadding));
  // define lower plugin section
  pluginAreaBottom = juce::Rectangle<int>(juce::Point<int>(windowArea.getX(), static_cast<int>(windowArea.getBottom() * (headerHeight + mainSectionTopHeight))), juce::Point<int>(windowArea.getRight(), windowArea.getBottom() * (1.0 - footerHeight)));
  int labelHeight = static_cast<int>(windowArea.getHeight()*0.08);
  knobArea = juce::Rectangle<int>(juce::Point<int>(pluginAreaBottom.getX(),pluginAreaBottom.getY()+labelHeight),juce::Point<int>(pluginAreaBottom.getRight(),pluginAreaBottom.getBottom()));

  auto boxAreaMain = getLocalBounds();
  auto boxAreaMainWidth = boxAreaMain.getWidth();
  auto boxAreaMainHeight = boxAreaMain.getHeight();

  resizeButton.setBounds(0, 0, static_cast<int>(boxAreaMainHeight * headerHeight * 3), static_cast<int>(boxAreaMainHeight * headerHeight));
  vintageButton.setBounds(resizeButton.getWidth(), 0, static_cast<int>(boxAreaMainHeight * headerHeight * 3), static_cast<int>(boxAreaMainHeight * headerHeight));
  noiseButton.setBounds(vintageButton.getWidth() + resizeButton.getWidth(), 0, static_cast<int>(boxAreaMainHeight * headerHeight * 3), static_cast<int>(boxAreaMainHeight * headerHeight));
  reverbClearButton.setBounds(noiseButton.getWidth() + vintageButton.getWidth() + resizeButton.getWidth(), 0, static_cast<int>(boxAreaMainHeight * headerHeight * 3), static_cast<int>(boxAreaMainHeight * headerHeight));
  urlButton.setBounds(static_cast<int>(boxAreaMainWidth * urlButtonScale[0]), 0, static_cast<int>(boxAreaMainWidth * urlButtonScale[1]), static_cast<int>(boxAreaMainHeight * headerHeight));
  boxAreaMain.removeFromTop(static_cast<int>(windowArea.getHeight() * headerHeight));
  juce::Rectangle<int> boxArea(juce::Point<int>(static_cast<int>(boxAreaMain.getRight() * 0.70833333f), static_cast<int>(boxAreaMain.getY() + boxAreaMain.getHeight() * 0.08333333f)), juce::Point<int>(static_cast<int>(boxAreaMain.getRight() * 0.95833333f), static_cast<int>(boxAreaMain.getY() + boxAreaMain.getHeight() * 0.25f)));
  
  programBox.setBounds(comboBoxArea1);

  int numKnobs = 6;
  int knobAreaWidth = static_cast<int>(knobArea.getWidth() / numKnobs);
  inputGainSlider.setBounds(knobArea.removeFromLeft(knobAreaWidth));
  lfdecaySlider.setBounds(knobArea.removeFromLeft(knobAreaWidth));
  hfdecaySlider.setBounds(knobArea.removeFromLeft(knobAreaWidth));
  mixSlider.setBounds(knobArea.removeFromLeft(knobAreaWidth));
  predelaySlider.setBounds(knobArea.removeFromLeft(knobAreaWidth));
  decaySlider.setBounds(knobArea.removeFromLeft(knobAreaWidth));
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
      redBox.setFontSize(static_cast<float>(fontSizeLarge * editorScale));
      customTextButton.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      customToggleButton.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      customKnobLabel.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      lfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      hfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      blueKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      whiteKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      redKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      setSize(static_cast<int>(defaultWidth * editorScale), static_cast<int>(defaultHeight * editorScale));
      resizeButton.setButtonText("150%");
    }
    else
    {
      editorScale = 1.0f;
      redBox.setFontSize(static_cast<float>(fontSizeLarge * editorScale));
      customTextButton.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      customToggleButton.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      customKnobLabel.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      inputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      lfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      hfdecaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      predelaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, static_cast<int>(textBoxWidth * editorScale), static_cast<int>(textBoxHeight * editorScale));
      blueKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      whiteKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      redKnob.setFontSize(static_cast<float>(fontSizeRegular * editorScale));
      setSize(static_cast<int>(defaultWidth * editorScale), static_cast<int>(defaultHeight * editorScale));
      resizeButton.setButtonText("100%");
    }
  }
}