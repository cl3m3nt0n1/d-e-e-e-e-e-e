#include "DelayComponent.hpp"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

DelayComponent::DelayComponent(juce::AudioProcessorValueTreeState& audioTree) :
    mbackgroundColour(juce::Colour::fromRGB(50, 222, 138)),
    apvts(audioTree),
    mDelayTimeSliderAttachment(apvts, "Delay Time", mDelayTimeSlider),
    mDelayFeedbackSliderAttachment(apvts, "Delay Feedback", mDelayFeedbackSlider)
{
    mDelayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mDelayFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

    mDelayTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mDelayFeedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    juce::FontOptions fontOptions("JetBrainsMono Nerd Font", "Bold", 18.0f);
    mDelayTimeSliderLabel.setFont(fontOptions);
    mDelayFeedbackSliderLabel.setFont(fontOptions);
    
    mDelayFeedbackSliderLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    mDelayTimeSliderLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    
    mDelayFeedbackSliderLabel.setJustificationType (juce::Justification::centredTop);
    mDelayTimeSliderLabel.setJustificationType (juce::Justification::centredTop);

    mDelayTimeSliderLabel.setText("DLY::TIM", juce::NotificationType::dontSendNotification);
    mDelayFeedbackSliderLabel.setText("DLY::FBK", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(mDelayFeedbackSlider);
    addAndMakeVisible(mDelayTimeSlider);
    addAndMakeVisible(mDelayFeedbackSliderLabel);
    addAndMakeVisible(mDelayTimeSliderLabel);

}

DelayComponent::~DelayComponent()
{
}

void DelayComponent::paint (juce::Graphics& g)
{   
    g.fillAll(mbackgroundColour);
    auto pathStroke = juce::PathStrokeType(6.0f, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
    auto area = juce::Path();
    area.addRectangle(getLocalBounds());
    g.setColour(juce::Colours::white);
    g.strokePath(area, pathStroke);
}

void DelayComponent::resized()
{
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows    = { 
        Track (Fr (1)), 
        Track (Fr (3)), 
        Track (Fr (1)), 
        Track (Fr (1)) 
        };
        
    grid.templateColumns = { 
        Track (Fr (1)), 
        Track (Fr (3)), 
        Track (Fr (1)), 
        Track (Fr (3)), 
        Track (Fr (1)) 
        };


    for (int i = 0; i < 25; ++i)
        grid.items.set(i, juce::GridItem(nullptr));


    grid.items.set(6, juce::GridItem(mDelayTimeSlider));
    grid.items.set(11, juce::GridItem(mDelayTimeSliderLabel));
    
    grid.items.set(8, juce::GridItem(mDelayFeedbackSlider));
    grid.items.set(13, juce::GridItem(mDelayFeedbackSliderLabel));
    
    grid.performLayout (getLocalBounds());
}