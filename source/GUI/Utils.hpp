#pragma once

#include "juce_core/juce_core.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct CompAndLabel : public juce::Component
{
public:
    CompAndLabel(juce::String labelText) 
    {
        juce::FontOptions fontOptions("JetBrainsMono Nerd Font", "Regular", 18.0f);
        label.setFont(fontOptions);
        label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
        label.setJustificationType(juce::Justification::centredTop);
        label.setText(labelText, juce::NotificationType::dontSendNotification);

        addAndMakeVisible(label);
    }

    void paint(juce::Graphics& g) override
    {

        juce::Path glyphPath;
        juce::GlyphArrangement glyphs;
        glyphs.addFittedText(label.getFont(), label.getText(), 
                             label.getBoundsInParent().getTopLeft().x, 
                             label.getBoundsInParent().getTopLeft().y, 
                             label.getBoundsInParent().getWidth(), 
                             label.getBoundsInParent().getHeight(), 
                             juce::Justification::centredTop, 10);
        glyphs.createPath(glyphPath);
        
        g.setColour(juce::Colours::white);
        juce::PathStrokeType strokeType(4.0f);
        g.strokePath(glyphPath, strokeType);
        g.fillPath(glyphPath);
        juce::DropShadow shadow;
        shadow.colour = juce::Colour(juce::uint8(0), juce::uint8(0), juce::uint8(0), 0.25f);
        shadow.drawForPath(g, glyphPath);
    }
protected:
    juce::Label label;
};


/**
 * @brief a basic struct inheriting from juce::component
 *        to hold a slider and a label arranged in a 
 *        simple grid way.
 * 
 */
struct SliderAndLabel : public CompAndLabel
{
public:
    SliderAndLabel(juce::String labelText) : CompAndLabel(labelText), slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                      juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        addAndMakeVisible(slider);
    }

    void resized() override
    {
        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = 
        {
            Track(Fr(4)),
            Track(Fr(1))
        };

        grid.templateColumns = 
        {
            Track(Fr(1))
        };

        grid.items.set(0, slider);
        grid.items.set(1, label);

        grid.performLayout(getLocalBounds());
    }

    juce::Slider& getslider() { return slider; }
    
    
private:
    juce::Slider slider;
};


/**
 * @brief Same thing but with a juce::ToggleButton.
 */
struct ToggleAndLabel : public CompAndLabel
{
public:
    ToggleAndLabel(juce::String toggleLabel): CompAndLabel(toggleLabel)
    {
        addAndMakeVisible(toggle);
    }

    void resized() override
    {
        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = 
        {
            Track(Fr(2)),
            Track(Fr(1))
        };

        grid.templateColumns = 
        {
            Track(Fr(1))
        };

        grid.items.set(0, toggle);
        grid.items.set(1, label);

        grid.performLayout(getLocalBounds());
    }

    juce::ToggleButton& getToggle() { return toggle; }
    
private:
    juce::ToggleButton toggle;
};
