#pragma once

#include "juce_graphics/juce_graphics.h"
#include <cmath>
#include <juce_gui_basics/juce_gui_basics.h>

class CustomLookNFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour (juce::Colours::black);
        g.fillEllipse (rx, ry, rw, rw);

        // outline
        g.setColour (juce::Colours::white);
        g.drawEllipse (rx, ry, rw, rw, 5.0f);

        // Drawing the pointer
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 4.0f;
        p.addRectangle (-pointerThickness * 0.3f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        g.setColour (juce::Colours::white);
        g.fillPath (p);

        // Add a drop shadow effect
        juce::Path ellipseAsPath;
        ellipseAsPath.addEllipse (rx, ry, rw, rw);
        juce::DropShadow shadow;
        shadow.colour = juce::Colour (juce::uint8 (0), juce::uint8 (0), juce::uint8 (0), 0.25f);
        shadow.radius = radius + 2;
        shadow.offset = juce::Point (3, 3);
        shadow.drawForPath (g, ellipseAsPath);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Point<float> topLeftOuterRectanglePoint (button.getLocalBounds().getTopLeft().x + button.getLocalBounds().getWidth() / 3,
            button.getLocalBounds().getTopLeft().y + button.getLocalBounds().getHeight() / 3);

        juce::Point<float> bottomRightOuterRectanglePoint (button.getLocalBounds().getBottomRight().x - button.getLocalBounds().getWidth() / 3,
            button.getLocalBounds().getBottomRight().y - button.getLocalBounds().getHeight() / 3);

        juce::Rectangle<float> outerToggleRectangle (topLeftOuterRectanglePoint, bottomRightOuterRectanglePoint);

        g.setColour (juce::Colours::white);
        g.drawRoundedRectangle (outerToggleRectangle, 7, 10);
        g.setColour (juce::Colours::black);
        g.drawRoundedRectangle (outerToggleRectangle, 7, 2);
        g.fillRoundedRectangle (outerToggleRectangle, 7);

        auto outerToggleRectangleCenter = outerToggleRectangle.getCentre();

        juce::Rectangle<float> ellipseOuterRectangle (outerToggleRectangle);
        ellipseOuterRectangle.removeFromLeft (ellipseOuterRectangle.getWidth() / 1.5);
        ellipseOuterRectangle.removeFromRight (ellipseOuterRectangle.getWidth() / 1.5);
        ellipseOuterRectangle.removeFromTop (ellipseOuterRectangle.getHeight() / 1.5);
        ellipseOuterRectangle.removeFromBottom (ellipseOuterRectangle.getHeight() / 1.5);
        ellipseOuterRectangle.setCentre (outerToggleRectangleCenter);

        auto ellipseLeftPosition = juce::Point<float> (ellipseOuterRectangle.getCentreX() + outerToggleRectangle.getWidth() / 5, ellipseOuterRectangle.getCentre().y);
        auto ellipseRightPosition = juce::Point<float> (ellipseOuterRectangle.getCentreX() - outerToggleRectangle.getWidth() / 5, ellipseOuterRectangle.getCentre().y);

        g.setColour (juce::Colours::white);
        if (button.getToggleState())
        {
            ellipseOuterRectangle.setCentre (ellipseLeftPosition);
            g.drawEllipse (ellipseOuterRectangle, 3);
            g.fillEllipse (ellipseOuterRectangle);
        }
        else
        {
            ellipseOuterRectangle.setCentre (ellipseRightPosition);
            g.drawEllipse (ellipseOuterRectangle, 3);
            g.fillEllipse (ellipseOuterRectangle);
        }
    }

private:
};
