#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <juce_audio_basics/juce_audio_basics.h>

class PresetManagerComponent 
{
public:

    PresetManagerComponent();

private:

    juce::TableListBox mListBox;
    
    

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManagerComponent)
};