#pragma once

#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace Utils
{
    // Writing it as a universal way to browse files.
    // On Linux:   ~/.config/{JucePlugin_Manufacturer}/{JucePlugin_Name}/Presets/
    // On MacOS:   ~/Library/{JucePlugin_Manufacturer}/{JucePlugin_Name}/Presets/
    // On Windows: \Documents and Settings\username\Application Data\{JucePlugin_Manufacturer}\{JucePlugin_Name}\Presets
    const juce::File PLUGIN_PRESET_PATH (juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory)
                                                                                                        .getChildFile(JucePlugin_Manufacturer)
                                                                                                        .getChildFile(JucePlugin_Name)
                                                                                                        .getChildFile("Presets"));
}


struct CompAndLabel : public juce::Component
{
public:
    CompAndLabel(juce::String labelText) /* : shadow(juce::Colour::fromRGBA(0, 0, 0, 30), 10, {}),
                                           shadower(shadow) */
    {
        juce::FontOptions fontOptions("JetBrainsMono NFM", "SemiBold", 18.0f);
        label.setFont(fontOptions.withKerningFactor(0.05));
        label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
        label.setJustificationType(juce::Justification::horizontallyCentred | juce::Justification::verticallyCentred);
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
                             juce::Justification::horizontallyCentred | juce::Justification::verticallyCentred, 1);
        glyphs.createPath(glyphPath);
        
        g.setColour(juce::Colours::white);
        juce::PathStrokeType strokeType(4.0f);
        g.strokePath(glyphPath, strokeType);
        g.fillPath(glyphPath);
    }
protected:
    juce::Label label;
/*     juce::DropShadow shadow;
    juce::DropShadower shadower; */
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
        // shadower.setOwner(dynamic_cast<Component*>(&slider));
        addAndMakeVisible(slider);
    }

    void resized() override
    {
        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = 
        {
            Track(Fr(3)),
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
        // shadower.setOwner(dynamic_cast<Component*>(&toggle));
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
