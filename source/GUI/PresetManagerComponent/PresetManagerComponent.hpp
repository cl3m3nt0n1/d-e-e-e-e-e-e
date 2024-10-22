#pragma once

#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"

class PresetManagerComponent 
{
public:

    PresetManagerComponent();

private:

    juce::ComboBox mComboBox;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManagerComponent)
};