#pragma once

#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"
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

        juce::DropShadow shadow (juce::Colour::fromRGBA(0, 0, 0, 64), 4, juce::Point(4, 4));

        juce::Path ellipseAsPath;
        ellipseAsPath.addEllipse (rx, ry, rw, rw);
        shadow.drawForPath(g, ellipseAsPath);

        // outline
        g.setColour (juce::Colours::white);
        g.drawEllipse (rx, ry, rw, rw, 5.0f);

        // fill
        g.setColour (juce::Colours::black);
        g.fillEllipse (rx, ry, rw, rw);

        // Drawing the pointer
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 4.0f;
        p.addRectangle (-pointerThickness * 0.3f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        g.setColour (juce::Colours::white);
        g.fillPath (p);

    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Point<float> topLeftOuterRectanglePoint (button.getLocalBounds().getTopLeft().x + button.getLocalBounds().getWidth() / 3,
            button.getLocalBounds().getTopLeft().y + button.getLocalBounds().getHeight() / 3);

        juce::Point<float> bottomRightOuterRectanglePoint (button.getLocalBounds().getBottomRight().x - button.getLocalBounds().getWidth() / 3,
            button.getLocalBounds().getBottomRight().y - button.getLocalBounds().getHeight() / 3);

        juce::Rectangle<float> outerToggleRectangle (topLeftOuterRectanglePoint, bottomRightOuterRectanglePoint);
        juce::Rectangle<float> outerToggleRectangleOutline(outerToggleRectangle);
        outerToggleRectangleOutline.expand(5, 5);
                
        g.setColour(juce::Colour::fromRGBA(0, 0, 0, 20));
        g.drawRoundedRectangle(outerToggleRectangleOutline.withTrimmedLeft(5).withTrimmedTop(5), 7, 6);

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

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &box) override
    {
        auto cornerSize = 20.0f;
        juce::Rectangle<int> boxBounds (0, 0, width, height);
        boxBounds.reduce(boxBounds.getWidth() / 12, boxBounds.getHeight() / 24);
        g.setColour (juce::Colour::fromRGBA(36, 22, 35, 255));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
        g.setColour (juce::Colour::fromFloatRGBA(251, 245, 243, 255));
        g.drawRoundedRectangle (boxBounds.toFloat(), cornerSize, 5.0f);
    }

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        label.setFont(label.getFont().withExtraKerningFactor(0.20f));
        label.setBounds(box.getLocalBounds().withSizeKeepingCentre(box.getWidth() / 4, box.getHeight() / 2));
        label.setJustificationType(juce::Justification::centred);
        label.setFont(fontOptions.withKerningFactor(0.05));
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool, bool isButtonDown) override
    {
        auto cornerSize = 20.0f;
        juce::Rectangle<int> boxBounds (0, 0, button.getWidth(), button.getHeight());
        boxBounds.reduce(boxBounds.getWidth() / 12, boxBounds.getHeight() / 24);
        g.setColour (juce::Colour::fromRGBA(36, 22, 35, 255));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
        g.setColour (juce::Colour::fromFloatRGBA(251, 245, 243, 255));
        g.drawRoundedRectangle (boxBounds.toFloat(), cornerSize, 5.0f);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        g.setFont (fontOptions.withKerningFactor(0.10f).withHeight(12.0f));
        g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                : juce::TextButton::textColourOffId)
                        .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
    
        auto yIndent = juce::jmin (4, button.proportionOfHeight (0.3f));
        auto cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;
    
        auto fontHeight = juce::roundToInt (fontOptions.getHeight() * 0.6f);
        auto leftIndent  = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft()  ? 4 : 2));
        auto rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        auto textWidth = button.getWidth() - leftIndent - rightIndent;
    
        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                            leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                            juce::Justification::centred, 2);
    }


private:
    juce::FontOptions fontOptions {"JetBrainsMono NFM", "Bold", 18.0f};

};
