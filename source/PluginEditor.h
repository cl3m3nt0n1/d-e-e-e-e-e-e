#pragma once

#include "BinaryData.h"
#include "GUI/Utils/Utils.hpp"
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Utils/CustomLookNFeel.hpp"
#include "GUI/DelayComponent/DelayComponent.hpp"
#include "GUI/ReverbComponent/ReverbComponent.hpp"
#include "GUI/PresetManagerComponent/PresetManagerComponent.hpp"

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

    juce::Label pluginName;

    DelayComponent delayComponent;
    ReverbComponent reverbComponent;
    SliderAndLabel mPluginDryWetSlider {  "DRY | WET" },
                   mPluginOutputLevel  {  "OUT::LVL"  },
                   mPluginOutputGain   {  "OUT::GAI"  };
    juce::AudioProcessorValueTreeState::SliderAttachment mPluginDryWetSliderAttachement,
                                                         mPluginOutputLevelAttachement,
                                                         mPluginOutputGainAttachement;

    PresetManagerComponent mPresetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
