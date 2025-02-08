#pragma once

#include <JuceHeader.h>

class RedBox : public juce::LookAndFeel_V4
{
public:
    RedBox()
    {
    }
    void drawComboBox(Graphics &g, int width, int height, bool,
                 int, int, int, int, ComboBox &box) override
    {
        auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(juce::Colour(0,0,0));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        g.setColour(box.findColour(ComboBox::outlineColourId));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

        Rectangle<int> arrowZone(width - 30, 0, 20, height);
        Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, PathStrokeType(2.0f));
    }
    void drawLabel(juce::Graphics &g, juce::Label &label) override
    {
        g.setColour(juce::Colour(255,0,0));
        g.drawText(label.getText(), label.getLocalBounds(), juce::Justification::centred);
    }
};

class BlueKnob : public juce::LookAndFeel_V4
{
public:
    BlueKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(juce::Colour(0, 102, 179));
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(juce::Colours::black);
        g.drawEllipse(centreX - (radius * 0.9f), centreY - (radius * 0.9f), radius * 1.8f, radius * 1.8f, radius * 0.2f);

        juce::Path p;
        auto pointerLength = radius * 0.9f;
        auto pointerThickness = radius * 0.15f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};

class WhiteKnob : public juce::LookAndFeel_V4
{
public:
    WhiteKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(juce::Colour(179, 148, 118));
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(juce::Colours::black);
        g.drawEllipse(centreX - (radius * 0.9f), centreY - (radius * 0.9f), radius * 1.8f, radius * 1.8f, radius * 0.2f);

        juce::Path p;
        auto pointerLength = radius * 0.9f;
        auto pointerThickness = radius * 0.15f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};

class RedKnob : public juce::LookAndFeel_V4
{
public:
    RedKnob()
    {
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(juce::Colour(179, 26, 26));
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(juce::Colours::black);
        g.drawEllipse(centreX - (radius * 0.9f), centreY - (radius * 0.9f), radius * 1.8f, radius * 1.8f, radius * 0.2f);

        juce::Path p;
        auto pointerLength = radius * 0.9f;
        auto pointerThickness = radius * 0.15f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};