#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
/**
 * @brief a basic struct inheriting from juce::component
 *        to hold a slider and a label arranged in a 
 *        simple grid way.
 * 
 */
struct SliderAndLabel : public juce::Component
{
public:
    SliderAndLabel(juce::String sliderLabel) :slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                      juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        juce::FontOptions fontOptions("JetBrainsMono Nerd Font", "Bold", 20.0f);
        
        label.setFont(fontOptions);
        label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
        label.setJustificationType(juce::Justification::centredTop);
        label.setText(sliderLabel, juce::NotificationType::dontSendNotification);

        addAndMakeVisible(slider);
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
                             juce::Justification::centredTop, 5);
        glyphs.createPath(glyphPath);
        
        g.setColour(juce::Colours::white);
        juce::PathStrokeType strokeType(4.0f);
        g.strokePath(glyphPath, strokeType);
        g.fillPath(glyphPath);
        juce::DropShadow shadow;
        shadow.colour = juce::Colour(juce::uint8(0), juce::uint8(0), juce::uint8(0), 0.25f);
        shadow.drawForPath(g, glyphPath);
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

        grid.items.set(0, slider);
        grid.items.set(1, label);

        grid.performLayout(getLocalBounds());
    }

    juce::Slider& getslider() { return slider; }
    
    
private:
    juce::Slider slider;
    juce::Label label;
};


/**
 * @brief Same thing but with a juce::ToggleButton.
 */
struct ToggleAndLabel : public juce::Component
{
public:
    ToggleAndLabel(juce::String sliderLabel)
    {
        juce::FontOptions fontOptions("JetBrainsMono Nerd Font", "Bold", 18.0f);
        
        label.setFont(fontOptions);
        label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
        label.setJustificationType(juce::Justification::centredTop);
        label.setText(sliderLabel, juce::NotificationType::dontSendNotification);
    
        addAndMakeVisible(toggle);
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
                             juce::Justification::centredTop, 5);
        glyphs.createPath(glyphPath);
        
        g.setColour(juce::Colours::white);
        juce::PathStrokeType strokeType(4.0f);
        g.strokePath(glyphPath, strokeType);
        g.fillPath(glyphPath);
        juce::DropShadow shadow;
        shadow.colour = juce::Colour(juce::uint8(0), juce::uint8(0), juce::uint8(0), 0.25f);
        shadow.drawForPath(g, glyphPath);
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
    juce::Label label;
};
