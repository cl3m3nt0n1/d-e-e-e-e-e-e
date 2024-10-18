#pragma once

#include "juce_graphics/juce_graphics.h"
#include <cmath>
#include <juce_gui_basics/juce_gui_basics.h>


class CustomLookNFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
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
        auto pointerLength = radius * log(1.3);
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerLength * 0.3f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        g.setColour (juce::Colours::white);
        g.fillPath (p);

        // Add a drop shadow effect
        juce::Path ellipseAsPath;
        ellipseAsPath.addEllipse (rx, ry, rw, rw);
        juce::DropShadow shadow;
        shadow.colour = juce::Colour(juce::uint8(0), juce::uint8(0),
            juce::uint8(0), 0.25f);
        shadow.radius = radius + 2;
        shadow.offset = juce::Point(3, 3);
        shadow.drawForPath (g, ellipseAsPath);


    }
private:

};
