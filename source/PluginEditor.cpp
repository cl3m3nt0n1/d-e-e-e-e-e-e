#include "PluginEditor.h"
#include "GUI/PresetManagerComponent/PresetManagerComponent.hpp"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

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

    juce::FontOptions options("JetBrainsMono Nerd Font", "Regular", 40);
    pluginName.setFont(juce::Font(options));
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

    /*
     * Delay component contour and drop shadow 
     */
    auto componentStroke = juce::PathStrokeType(6.0f, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
    auto delayComponentPath = juce::Path();
    delayComponentPath.addRoundedRectangle(delayComponent.getBounds().getX(),
                                           delayComponent.getBounds().getY(), 
                                           delayComponent.getBounds().getWidth()  + 3, 
                                           delayComponent.getBounds().getHeight() + 3, 
                                           2.0f);
    g.setColour(juce::Colours::white);
    g.strokePath(delayComponentPath, componentStroke);
   /*  juce::DropShadow delayShadow;
    delayShadow.colour = juce::Colour(juce::uint8(0), juce::uint8(0), juce::uint8(0), 0.25f);
    delayShadow.offset = juce::Point(5,5);
    delayShadow.drawForPath(g, delayComponentPath);
 */
    /*
     * Reverb component contour and drop shadow 
     */
    auto reverbComponentPath = juce::Path();
    reverbComponentPath.addRoundedRectangle(reverbComponent.getBounds().getX(),
                                            reverbComponent.getBounds().getY(), 
                                            reverbComponent.getBounds().getWidth()  + 3, 
                                            reverbComponent.getBounds().getHeight() + 3, 
                                            2.0f);
    g.setColour(juce::Colours::white);
    g.strokePath(reverbComponentPath, componentStroke);
/*     juce::DropShadow reverbShadow;
    reverbShadow.colour = juce::Colour(juce::uint8(0), juce::uint8(0), juce::uint8(0), 0.25f);
    reverbShadow.offset = juce::Point(10,10);
    reverbShadow.drawForPath(g, reverbComponentPath); */

    /*
     * Dry/Wet contour and drop shadow 
     */
    // auto dryWetPath = juce::Path();
    // dryWetPath.addRoundedRectangle(mPluginDryWetSlider.getBounds(), 2.0f);
    // g.setColour(juce::Colours::white);
    // g.strokePath(dryWetPath, componentStroke);
    /* juce::DropShadow dryWetShadow;
    dryWetShadow.colour = juce::Colour(juce::uint8(0), juce::uint8(0), juce::uint8(0), 0.25f);
    dryWetShadow.offset = juce::Point(5,5);
    dryWetShadow.drawForPath(g, dryWetPath) */;
}

void PluginEditor::resized()
{
    #if DEBUG
    inspectButton.setBounds (0, 0, 50, 25);
    #endif

    auto pluginNameArea = getBounds();
    pluginNameArea.removeFromTop(10);
    pluginNameArea.removeFromBottom(7 * getHeight() / 8);
    
    pluginName.setBounds(pluginNameArea);

    auto presetArea = getBounds();
    presetArea.removeFromTop(70);
    presetArea.removeFromBottom(4 * getHeight() / 5);
    mPresetManager.setBounds(presetArea.withSizeKeepingCentre(6 * presetArea.getWidth() / 8, presetArea.getHeight()));
    

    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.columnGap = juce::Grid::Px(40);
    grid.rowGap    = juce::Grid::Px(20);

    grid.templateRows    = { 
                             Track (Fr (0)), // to center everything
                             Track (Fr (5)), // Main Content
                             Track (Fr (10)), // Main Content
                             Track (Fr (0))
                             }; // to center everything
    
    grid.templateColumns = { 
                             Track (Fr (0)), // to center everything
                             Track (Fr (30)), // Main Content
                             Track (Fr (20)), // Main Content
                             Track (Fr (20)), // Main Content
                             Track (Fr (0)) 
                            }; // to center everything 

    for (int i = 0; i < 20; ++i)
        grid.items.set(i, juce::GridItem(nullptr));

    grid.items.set(6, juce::GridItem(delayComponent));
    grid.items.set(11, juce::GridItem(reverbComponent));
    
    grid.items.set(8, juce::GridItem(mPluginOutputLevel));
    grid.items.set(13, juce::GridItem(mPluginOutputGain).withMargin(30));
    grid.items.set(12, juce::GridItem(mPluginDryWetSlider).withMargin(30));

    auto gridArea = getBounds();
    gridArea.removeFromTop(120);
    grid.performLayout (gridArea);


}   