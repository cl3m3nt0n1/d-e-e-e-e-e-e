#include "PluginEditor.h"
#include "CustomLookNFeel/CustomLookNFeel.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    ReverbDampingSliderAttachment(processorRef.getApvts(), "Reverb Damping", ReverbDampingSlider),
    ReverbRoomSizeSliderAttachment(processorRef.getApvts(), "Reverb Room Size", ReverbRoomSizeSlider),
    ReverbWetSliderAttachment(processorRef.getApvts(), "Reverb Wet", ReverbWetSlider),
    ReverbDrySliderAttachment(processorRef.getApvts(), "Reverb Dry", ReverbDrySlider),
    ReverbWidthSliderAttachment(processorRef.getApvts(), "Reverb Width", ReverbWidthSlider),
    PluginDryWetSliderAttachment(processorRef.getApvts(), "Plugin Dry Wet", PluginDryWetSlider),
    delayComponent(processorRef.getApvts())
{
    juce::ignoreUnused (processorRef);

    setLookAndFeel(new CustomLookNFeel);

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

       // Display our sliders
    for (auto* component : getAllSliders())
        addAndMakeVisible (component);

    addAndMakeVisible(delayComponent);    

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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto bounds = getBounds();

    juce::ColourGradient gradient;
    auto horizontalGradient = gradient.horizontal(juce::Colours::pink, juce::Colours::green, bounds);
    horizontalGradient.multiplyOpacity(0);
    g.setGradientFill (horizontalGradient);
    g.fillRect (bounds);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    auto sliders = getAllSliders();
    
    #if DEBUG
    area.removeFromBottom (50);
    inspectButton.setBounds (getLocalBounds().withSizeKeepingCentre (100, 50));
    #endif


    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows    = { 
                             Track (Fr (1)), // to center everything
                             Track (Fr (2)), // Deeeeee
                             Track (Fr (5)), // Main Content
                             Track (Fr (5)), // Main Content
                             Track (Fr (1))
                             }; // to center everything
    
    grid.templateColumns = { 
                             Track (Fr (1)), // to center everything
                             Track (Fr (6)), //Main Content
                             Track (Fr (4)), // Main Content
                             Track (Fr (6)), // Main Content
                             Track (Fr (1)) 
                            }; // to center everything 

    for (int i = 0; i < 25; ++i)
        grid.items.set(i, juce::GridItem(nullptr));


    grid.items.set(11, juce::GridItem(delayComponent));

    grid.performLayout (getLocalBounds());


}   

std::vector<juce::Component*> PluginEditor::getReverbSliders()
{
    // Very convenient way of returning a std::vector.
    return {
        &ReverbDampingSlider,
        &ReverbRoomSizeSlider,
        &ReverbWetSlider,
        &ReverbDrySlider,
        &ReverbWidthSlider        
    };
}

/* std::vector<juce::Component*> PluginEditor::getDelaySliders()
{
    return {
        &DelayTimeSlider,
        &DelayFeedbackSlider
    };
} */

std::vector<juce::Component*> PluginEditor::getAllSliders()
{
    return {
        &ReverbDampingSlider,
        &ReverbRoomSizeSlider,
        &ReverbWetSlider,
        &ReverbDrySlider,
        &ReverbWidthSlider,
        &PluginDryWetSlider
    };
}
