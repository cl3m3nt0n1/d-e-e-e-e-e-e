#include "DelayComponent.hpp"
#include "juce_graphics/juce_graphics.h"

DelayComponent::DelayComponent (juce::AudioProcessorValueTreeState& audioTree) : mbackgroundColour (juce::Colour::fromRGB (50, 222, 138)),
                                                                                 apvts (audioTree),
                                                                                 mDelayTimeSliderAttachment (apvts, "Delay Time", mDelayTimeSlider.getslider()),
                                                                                 mDelayFeedbackSliderAttachment (apvts, "Delay Feedback", mDelayFeedbackSlider.getslider()),
                                                                                 mDelaySyncSliderAttachment (apvts, "Delay Sync", mDelaySyncSlider.getslider()),
                                                                                 mDelaySyncToggleAttachment(apvts, "Delay Sync Toggle", mDelaySyncToggle.getToggle())
{
    addAndMakeVisible (mDelayFeedbackSlider);
    addAndMakeVisible (mDelayTimeSlider);
    addAndMakeVisible (mDelaySyncSlider);
    addAndMakeVisible (mDelaySyncToggle);
}

DelayComponent::~DelayComponent()
{
}

void DelayComponent::paint (juce::Graphics& g)
{
    g.fillAll (mbackgroundColour);

    auto pathStroke = juce::PathStrokeType (6.0f, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
    auto area = juce::Path();

/* 
    area.addRoundedRectangle(getLocalBounds(), 10.0f);
    g.setColour(juce::Colours::white);
    g.strokePath(area, pathStroke);
 */
}

void DelayComponent::resized()
{
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows = {
        Track (Fr (1)),
        Track (Fr (5)),
        Track (Fr (5))
    };

    grid.templateColumns = {
        Track (Fr (1)),
        Track (Fr (10)),
        Track (Fr (10)),
        Track (Fr (1))
    };

    for (int i = 0; i < 12; ++i)
        grid.items.set (i, juce::GridItem (nullptr));

    grid.items.set (10, juce::GridItem (mDelaySyncToggle));
    grid.items.set (5, juce::GridItem (mDelayTimeSlider));                             
    grid.items.set (9, juce::GridItem (mDelaySyncSlider));
    grid.items.set (6, juce::GridItem (mDelayFeedbackSlider));

    grid.performLayout (getLocalBounds());
}