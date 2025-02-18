#pragma once

#include <JuceHeader.h>

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