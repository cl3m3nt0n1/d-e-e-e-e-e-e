#include "ReverbComponent.hpp"

ReverbComponent::ReverbComponent(juce::AudioProcessorValueTreeState& audioTree) :
    mbackgroundColour(juce::Colour::fromRGB(225, 90, 151)),
    apvts(audioTree),
    mReverbDampingSliderAttachement(apvts, "Reverb Damping", mReverbDampingSlider.getslider()),
    mReverbRoomSizeSliderAttachement(apvts, "Reverb Room Size",mReverbRoomSizeSlider.getslider()),
    mReverbWetSliderAttachement(apvts, "Reverb Wet", mReverbWetSlider.getslider()), 
    mReverbDrySliderAttachement(apvts, "Reverb Dry", mReverbDrySlider.getslider()),
    mReverbWidthSliderAttachement(apvts, "Reverb Width",mReverbWidthSlider.getslider()),
    mReverbFreezeToggleAttachement(apvts, "Reverb Freeze", mReverbFreezeToggle.getToggle())
{

    addAndMakeVisible(mReverbFreezeToggle);
    addAndMakeVisible(mReverbDampingSlider );
    addAndMakeVisible(mReverbRoomSizeSlider);
    addAndMakeVisible(mReverbWetSlider );
    addAndMakeVisible(mReverbDrySlider);
    addAndMakeVisible(mReverbWidthSlider);
}

ReverbComponent::~ReverbComponent()
{
}

void ReverbComponent::paint (juce::Graphics& g)
{
    g.fillAll(mbackgroundColour);
    auto pathStroke = juce::PathStrokeType(6.0f, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
    auto area = juce::Path();
    area.createPathWithRoundedCorners(20);
    area.addRectangle(getLocalBounds());
    g.setColour(juce::Colours::white);
    g.strokePath(area, pathStroke);
}

void ReverbComponent::resized()
{
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.alignContent = juce::Grid::AlignContent::spaceEvenly;
    grid.alignItems = juce::Grid::AlignItems::center;

    grid.templateRows = { 
        Track (Fr (1)), 
        Track (Fr (10)), 
        Track (Fr (10)), 
        Track (Fr (10)), 
        Track (Fr (1))
        };
        
    grid.templateColumns = { 
        Track (Fr (1)), 
        Track (Fr (50)), 
        Track (Fr (10)), 
        Track (Fr (50)), 
        Track (Fr (1))
    };

    for (int i = 0; i < 25; ++i)
        grid.items.set(i, juce::GridItem(nullptr));

    
    grid.items.set(6,  mReverbDampingSlider);
    grid.items.set(8,  mReverbFreezeToggle);
    grid.items.set(11, mReverbDrySlider);
    grid.items.set(13, mReverbWetSlider);
    grid.items.set(16, mReverbRoomSizeSlider);
    grid.items.set(18, mReverbWidthSlider);

    grid.performLayout (getLocalBounds());
}

std::vector<juce::Component*> ReverbComponent::getSliders()
{
    return {
        &mReverbDampingSlider,
        &mReverbRoomSizeSlider,
        &mReverbWetSlider,
        &mReverbDrySlider,
        &mReverbWidthSlider
    };
}
