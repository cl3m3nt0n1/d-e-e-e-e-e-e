#include <juce_gui_basics/juce_gui_basics.h>
#include "PresetListBoxModel.hpp"

class PresetListBox
{
public:
    PresetListBox()
    {
        mPresetList.setModel(dynamic_cast<juce::ListBoxModel*>(&model));

        
    }
private:
    PresetListBoxModel model;
    juce::ListBox mPresetList;
};