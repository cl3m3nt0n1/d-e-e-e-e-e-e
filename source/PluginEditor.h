#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include <juce_audio_processors/juce_audio_processors.h>

#include "CustomLookNFeel/CustomLookNFeel.h"
#include "DelayComponent/DelayComponent.hpp"



/**
 * @brief a structure to ease the Slider creation process.
 * Every CustomRotarySlider will be the same
 * and each one of them suits our needs.
 * */
struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                             juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    {
    }
};

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    /*======================== METHODS ===========================*/
    /**
    * @brief getter for Sliders.
    * @returns a vector containing every sliders related to the reverb.
    * */
    std::vector<juce::Component*> getReverbSliders();

    /**
    * @brief getter for Sliders.
    * @returns a vector containing every sliders related to the delay.
    * */
    std::vector<juce::Component*> getDelaySliders();

    /**
    * @brief getter for Sliders.
    * @returns a vector containing every of the UI.
    * */
    std::vector<juce::Component*> getAllSliders();

    

    /*======================== MEMBERS ===========================*/
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    
    #if DEBUG
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    #endif

    DelayComponent delayComponent;

    CustomRotarySlider ReverbDampingSlider,
        ReverbRoomSizeSlider,
        ReverbWetSlider,
        ReverbDrySlider,
        ReverbWidthSlider,
        PluginDryWetSlider;

    juce::ToggleButton ReverbFreezeButton;


    juce::AudioProcessorValueTreeState::SliderAttachment
    ReverbDampingSliderAttachment,
    ReverbRoomSizeSliderAttachment,
    ReverbWetSliderAttachment,
    ReverbDrySliderAttachment,
    ReverbWidthSliderAttachment,
    PluginDryWetSliderAttachment;
    
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
