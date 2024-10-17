#include "DelayComponent.hpp"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

DelayComponent::DelayComponent(juce::AudioProcessorValueTreeState& audioTree) :
    backgroundColour(juce::Colours::green),
    apvts(audioTree),
    DelayTimeSliderAttachment(apvts, "Delay Time", DelayTimeSlider),
    DelayFeedbackSliderAttachment(apvts, "Delay FeedBack", DelayFeedbackSlider)
{
    setLookAndFeel(new CustomLookNFeel);
    addAndMakeVisible(DelayFeedbackSlider);
    addAndMakeVisible(DelayTimeSlider);
}

DelayComponent::~DelayComponent()
{
    setLookAndFeel(nullptr);
}

void DelayComponent::paint (juce::Graphics& g)
{
    g.fillAll(backgroundColour);
}

void DelayComponent::resized()
{
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows    = { Track (Fr (1)) };
    grid.templateColumns = { Track (Fr (1)), Track (Fr (1)) };

    grid.items = { juce::GridItem (DelayTimeSlider), juce::GridItem (DelayTimeSlider) };

    grid.performLayout (getLocalBounds());
}