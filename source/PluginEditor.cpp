#include "PluginEditor.h"
#include "juce_graphics/juce_graphics.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    delayComponent(processorRef.getApvts()),
    reverbComponent(processorRef.getApvts())
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

    addAndMakeVisible(delayComponent);    
    addAndMakeVisible(reverbComponent);    

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
    g.strokePath(firstTriangle, pathStroke);
    
    g.setColour(juce::Colour::fromRGB(50, 222, 138));
    g.fillPath (secondTriangle);
    g.setColour(juce::Colours::white);
    g.strokePath(secondTriangle, pathStroke);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();

    #if DEBUG
    area.removeFromBottom (50);
    inspectButton.setBounds (getLocalBounds().withSizeKeepingCentre (100, 50));
    #endif


    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.setGap(juce::Grid::Px(10));

    grid.templateRows    = { 
                             Track (Fr (1)), // to center everything
                             Track (Fr (2)), // Deeeeee
                             Track (Fr (5)), // Main Content
                             Track (Fr (5)), // Main Content
                             Track (Fr (1))
                             }; // to center everything
    
    grid.templateColumns = { 
                             Track (Fr (1)), // to center everything
                             Track (Fr (6)), // Main Content
                             Track (Fr (4)), // Main Content
                             Track (Fr (6)), // Main Content
                             Track (Fr (1)) 
                            }; // to center everything 

    for (int i = 0; i < 25; ++i)
        grid.items.set(i, juce::GridItem(nullptr));


    grid.items.set(11, juce::GridItem(delayComponent));
    grid.items.set(16, juce::GridItem(reverbComponent));

    grid.performLayout (getLocalBounds());


}   