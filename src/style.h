#pragma once

#include <JuceHeader.h>
//==============================================================================
class CustomTextButton : public juce::LookAndFeel_V4
{
private:
    float mTextButtonFontSize;

public:
    CustomTextButton()
    {
    }
    void setFontSize(float newSize);
    juce::Font getTextButtonFont(juce::TextButton &, int buttonHeight) override;

    void drawButtonBackground(juce::Graphics &, juce::Button &, const juce::Colour &backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
//==============================================================================
class CustomToggleButton : public juce::LookAndFeel_V4
{
private:
    float mToggleButtonFontSize;

public:
    CustomToggleButton()
    {
    }
    void setFontSize(float newSize);
    void drawToggleButton(juce::Graphics &, juce::ToggleButton &,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawTickBox(juce::Graphics &, juce::Component &,
                     float x, float y, float w, float h,
                     bool ticked, bool isEnabled,
                     bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void changeToggleButtonWidthToFitText(juce::ToggleButton &) override;
};
//==============================================================================
class RedBox : public juce::LookAndFeel_V4
{
private:
    float mBoxLabelFontSize;

public:
    RedBox()
    {
    }
    void setFontSize(float newSize);
    void drawComboBox(juce::Graphics &, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox &) override;
    juce::Font getComboBoxFont(juce::ComboBox &) override;
    void positionComboBoxText(juce::ComboBox &, juce::Label &) override;
};
//==============================================================================
class CustomKnobLabel : public juce::LookAndFeel_V2
{
private:
    float mKnobLabelFontSize;

public:
    CustomKnobLabel()
    {
    }

    void setFontSize(float newSize);
    void drawLabel(juce::Graphics &, juce::Label &) override;
    juce::Font getLabelFont(juce::Label &) override;
    juce::BorderSize<int> getLabelBorderSize(juce::Label &) override;
};
//==============================================================================
class BlueKnob : public juce::LookAndFeel_V4
{
public:
    BlueKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;
};
//==============================================================================
class WhiteKnob : public juce::LookAndFeel_V4
{
public:
    WhiteKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;
};
//==============================================================================
class RedKnob : public juce::LookAndFeel_V4
{
public:
    RedKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;
};