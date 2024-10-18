#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/CustomLookNFeel.hpp"
#include "GUI/DelayComponent/DelayComponent.hpp"
#include "GUI/ReverbComponent/ReverbComponent.hpp"

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
    /*======================== MEMBERS ===========================*/
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    CustomLookNFeel customLook;
    
    #if DEBUG
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect" };
    #endif

    DelayComponent delayComponent;
    ReverbComponent reverbComponent;
    SliderAndLabel mPluginDryWetSlider {"DRY | WET"};
    juce::AudioProcessorValueTreeState::SliderAttachment mPluginDryWetSliderAttachement;
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
