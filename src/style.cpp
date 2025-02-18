#include "style.h"

void CustomButton::setFontSize(float newSize)
{
    mButtonFontSize = newSize;
}
Font CustomButton::getTextButtonFont(TextButton &, int buttonHeight)
{
    return withDefaultMetrics(FontOptions(mButtonFontSize));
}

void CustomButton::drawButtonBackground(Graphics &g,
                                        Button &button,
                                        const Colour &backgroundColour,
                                        bool shouldDrawButtonAsHighlighted,
                                        bool shouldDrawButtonAsDown)
{
    auto cornerSize = 6.0f;
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                          .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour(baseColour);

    auto flatOnLeft = button.isConnectedOnLeft();
    auto flatOnRight = button.isConnectedOnRight();
    auto flatOnTop = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        Path path;
        path.addRoundedRectangle(bounds.getX(), bounds.getY(),
                                 bounds.getWidth(), bounds.getHeight(),
                                 cornerSize, cornerSize,
                                 !(flatOnLeft || flatOnTop),
                                 !(flatOnRight || flatOnTop),
                                 !(flatOnLeft || flatOnBottom),
                                 !(flatOnRight || flatOnBottom));

        g.fillPath(path);

        g.setColour(button.findColour(ComboBox::outlineColourId));
        g.strokePath(path, PathStrokeType(1.0f));
    }
    else
    {
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(button.findColour(ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }
}

void RedBox::drawComboBox(Graphics &g, int width, int height, bool,
                          int, int, int, int, ComboBox &box)
{
    auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(juce::Colour(24, 24, 24));
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
void RedBox::setFontSize(float newSize)
{
    mLabelFontSize = newSize;
}
Font RedBox::getComboBoxFont(ComboBox &box)
{
    return withDefaultMetrics(FontOptions(mLabelFontSize));
}
void RedBox::positionComboBoxText(ComboBox &box, Label &label)
{
    label.setBounds(1, 1,
                    box.getWidth() - 30,
                    box.getHeight() - 2);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(255, 0, 0));
    label.setFont(getComboBoxFont(box));
}

void BlueKnob::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                                const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &)
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

void WhiteKnob::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                                 const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &)
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

void RedKnob::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                               const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &)
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