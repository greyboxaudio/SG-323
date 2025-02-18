#pragma once

#include <JuceHeader.h>

class CustomButton : public juce::LookAndFeel_V4
{
private:
    float mButtonFontSize;

public:
    CustomButton()
    {
    }
    void setFontSize(float newSize);
    juce::Font getTextButtonFont(juce::TextButton &, int buttonHeight) override;

    void drawButtonBackground(juce::Graphics &, juce::Button &, const juce::Colour &backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class RedBox : public juce::LookAndFeel_V4
{
private:
    float mLabelFontSize;

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

class BlueKnob : public juce::LookAndFeel_V4
{
public:
    BlueKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;
};

class WhiteKnob : public juce::LookAndFeel_V4
{
public:
    WhiteKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;
};

class RedKnob : public juce::LookAndFeel_V4
{
public:
    RedKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;
};