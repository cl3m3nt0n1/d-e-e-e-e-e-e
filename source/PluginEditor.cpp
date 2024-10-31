#include "PluginEditor.h"
#include "GUI/PresetManagerComponent/PresetManagerComponent.hpp"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <cstddef>

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    delayComponent(processorRef.getApvts()),
    reverbComponent(processorRef.getApvts()),
    mPluginDryWetSliderAttachement(processorRef.getApvts(), "Plugin Dry Wet", mPluginDryWetSlider.getslider()),
    mPluginOutputLevelAttachement(processorRef.getApvts(), "Output Level", mPluginOutputLevel.getslider()),
    mPluginOutputGainAttachement(processorRef.getApvts(), "Output Gain", mPluginOutputGain.getslider()),
    mPresetManager(processorRef.getApvts())
{
    juce::ignoreUnused (processorRef);

    setLookAndFeel(&customLook);

    #if DEBUG
    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };
    #endif

    juce::FontOptions options("JetBrainsMono Nerd Font", "Bold", 60);
    pluginName.setColour(juce::Label::ColourIds::textColourId, juce::Colour::fromRGB(245, 245, 245));
    pluginName.setFont(juce::Font(options.withKerningFactor(0.40)));
    pluginName.setText("Deeeeee", juce::NotificationType::dontSendNotification);
    pluginName.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(pluginName);
    addAndMakeVisible(delayComponent);    
    addAndMakeVisible(reverbComponent);   
    addAndMakeVisible(mPluginDryWetSlider); 
    addAndMakeVisible(mPluginOutputLevel); 
    addAndMakeVisible(mPluginOutputGain); 
    addAndMakeVisible(mPresetManager); 

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel (nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    auto area = getBounds();

    /*
     * BACKGROUND RENDERING 
     */
    juce::Path firstTriangle;
    juce::Path secondTriangle;
    auto pathStroke = juce::PathStrokeType(6.0f);
    
    firstTriangle.addTriangle (area.getBottomLeft().toFloat(), area.getTopLeft().toFloat(), area.getTopRight().toFloat());
    secondTriangle.addTriangle (area.getBottomRight().toFloat(), area.getBottomLeft().toFloat(), area.getTopRight().toFloat());

    g.setColour(juce::Colour::fromRGB(225, 90, 151));
    g.fillPath (firstTriangle);
    g.setColour(juce::Colours::white);
    // g.strokePath(firstTriangle, pathStroke);
    
    g.setColour(juce::Colour::fromRGB(50, 222, 138));
    g.fillPath (secondTriangle);
    g.setColour(juce::Colours::white);
    // g.strokePath(secondTriangle, pathStroke);




    // DropShadow for every Component.
    juce::DropShadow shadow;
    shadow.colour = juce::Colour::fromRGBA(0, 0, 0, 64);
    shadow.radius = 8; // Default is 4
    shadow.offset = juce::Point<int>(8, 8);

    /*
     * Delay component contour and drop shadow 
     */
    auto componentStroke = juce::PathStrokeType(8.0f, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
    auto delayComponentPath = juce::Path();
    float cornerSize = 4.0f;
    delayComponentPath.addRoundedRectangle(delayComponent.getBounds(), cornerSize);
    shadow.drawForPath(g, delayComponentPath);
    g.setColour(juce::Colours::white);
    g.strokePath(delayComponentPath, componentStroke);

    /*
     * Reverb component contour and drop shadow 
     */
    auto reverbComponentPath = juce::Path();
    reverbComponentPath.addRoundedRectangle(reverbComponent.getBounds(), cornerSize);
    shadow.drawForPath(g, reverbComponentPath);
    g.setColour(juce::Colours::white);
    g.strokePath(reverbComponentPath, componentStroke);


    /*
     * Level contour and drop shadow 
     */
    auto levelPath = juce::Path();
    levelPath.addRoundedRectangle(mPluginOutputLevel.getBounds(), cornerSize);
    shadow.drawForPath(g, levelPath);
    g.setColour(juce::Colours::white);
    g.strokePath(levelPath, componentStroke);
    g.setColour(juce::Colour::fromRGB(50, 222, 138));
    g.fillPath(levelPath);


    
    /*
     * Gain contour and drop shadow 
     */
    auto gainPath = juce::Path();
    gainPath.addRoundedRectangle(mPluginOutputGain.getBounds(), cornerSize);
    shadow.drawForPath(g, gainPath);
    g.setColour(juce::Colours::white);
    g.strokePath(gainPath, componentStroke);
    g.setColour(juce::Colour::fromRGB(225, 90, 151));
    g.fillPath(gainPath);

    /*
     * Dry/Wet contour and drop shadow 
     */
    auto dryWetPath = juce::Path();
    dryWetPath.addRoundedRectangle(mPluginDryWetSlider.getBounds(), cornerSize);
    shadow.drawForPath(g, dryWetPath);
    g.setColour(juce::Colours::white);
    g.strokePath(dryWetPath, componentStroke);
    g.setColour(juce::Colour::fromRGB(225, 90, 151));
    g.fillPath(dryWetPath);

}

void PluginEditor::resized()
{
    #if DEBUG
    inspectButton.setBounds (0, 0, 50, 25);
    #endif

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    // Two main areas for utilities and parameters. 
    auto gridArea = getBounds();
    auto utilsArea = gridArea.removeFromTop(120);

    /*
     * Utilities grid. 
     */
    juce::Grid utilsGrid;
    
    utilsGrid.rowGap = juce::Grid::Px(10);

    utilsGrid.templateColumns = { 
        Track (Fr (1)),
        Track (Fr (20)),
        Track (Fr (1)),
    };
    
    utilsGrid.templateRows = { 
        Track (Fr (10)),
        Track (Fr (8)),
    };

    for(int i = 0; i < 6; ++i)
        utilsGrid.items.set(i, nullptr);
    
    utilsGrid.items.set(1, pluginName);
    utilsGrid.items.set(4, juce::GridItem(mPresetManager));

    utilsGrid.performLayout(utilsArea.withSizeKeepingCentre(7 * utilsArea.getWidth() / 8, utilsArea.getHeight()));


    /*
     * Parameters Grid.
     */
    juce::Grid parameterGrid;

    parameterGrid.columnGap = juce::Grid::Px(30);
    parameterGrid.rowGap    = juce::Grid::Px(10);

    parameterGrid.templateRows    = { 
                             Track (Fr (0)), // to center everything
                             Track (Fr (6)), // Main Content
                             Track (Fr (10)), // Main Content
                             Track (Fr (0))
                             }; // to center everything
    
    parameterGrid.templateColumns = { 
                             Track (Fr (0)), // to center everything
                             Track (Fr (30)), // Main Content
                             Track (Fr (20)), // Main Content
                             Track (Fr (20)), // Main Content
                             Track (Fr (0)) 
                            }; // to center everything 

    for (int i = 0; i < 20; ++i)
        parameterGrid.items.set(i, juce::GridItem(nullptr));

    parameterGrid.items.set(6, juce::GridItem(delayComponent).withMargin(10));
    parameterGrid.items.set(11, juce::GridItem(reverbComponent).withMargin(10));
    parameterGrid.items.set(8, juce::GridItem(mPluginOutputLevel).withMargin(10));
    parameterGrid.items.set(13, juce::GridItem(mPluginOutputGain).withMargin(10));
    parameterGrid.items.set(12, juce::GridItem(mPluginDryWetSlider).withMargin(10));


    parameterGrid.performLayout (gridArea);


}   