#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_core/system/juce_PlatformDefs.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

#include "../Utils.hpp"

/**
 * @brief The juce::component related to the reverb
 *        instance of the plugin.
 */
class ReverbComponent : public juce::Component
{
public:
    ReverbComponent (juce::AudioProcessorValueTreeState& audioTree);

    ~ReverbComponent() override;

    void paint (juce::Graphics& g) override;

    void resized() override;

    std::vector<juce::Component*> getSliders();

private:
    juce::Colour mbackgroundColour;

    juce::AudioProcessorValueTreeState& apvts;

    // Reverb sliders and everything related
    SliderAndLabel mReverbDampingSlider { "REV::DMP" },
                   mReverbRoomSizeSlider { "REV::RSZ" },
                   mReverbWetSlider { "REV::WET" },
                   mReverbDrySlider { "REV::DRY" },
                   mReverbWidthSlider { "REV::WID" };

    juce::AudioProcessorValueTreeState::SliderAttachment mReverbDampingSliderAttachement,
                                                         mReverbRoomSizeSliderAttachement,
                                                         mReverbWetSliderAttachement,
                                                         mReverbDrySliderAttachement,
                                                         mReverbWidthSliderAttachement;

    // Reverb Freeze Toggle and everything related
    ToggleAndLabel mReverbFreezeToggle { "REV::FRZ" };
    juce::AudioProcessorValueTreeState::ButtonAttachment
        mReverbFreezeToggleAttachement;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbComponent)
};