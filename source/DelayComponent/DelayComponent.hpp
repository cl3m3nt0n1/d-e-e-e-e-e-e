#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "../CustomLookNFeel/CustomLookNFeel.h"

class DelayComponent : public juce::Component
{
public:

    DelayComponent(juce::AudioProcessorValueTreeState& audioTree);

    ~DelayComponent() override;

    void paint (juce::Graphics& g) override;
    
    void resized() override;

private:
    juce::Colour backgroundColour;
    
    juce::AudioProcessorValueTreeState& apvts;

    juce::Slider DelayTimeSlider, DelayFeedbackSlider;
    
    juce::AudioProcessorValueTreeState::SliderAttachment
        DelayTimeSliderAttachment,
        DelayFeedbackSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayComponent)
};