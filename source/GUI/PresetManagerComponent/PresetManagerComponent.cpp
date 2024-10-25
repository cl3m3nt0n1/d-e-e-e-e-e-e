#include "PresetManagerComponent.hpp"
#include "juce_core/system/juce_PlatformDefs.h"
#include <memory>


PresetManagerComponent::PresetManagerComponent()
{
    mPreviousButton.setButtonText("Previous");
    mNextButton.setButtonText("Next");
    mSaveButton.setButtonText("Save");
    mDeleteButton.setButtonText("Delete");

    mComboBox.setTextWhenNothingSelected("Select preset");

    auto preset = loadPresetFromXML(juce::File("/home/clementone/Documents/DEVS/d-e-e-e-e-e-e/Resources/preset_test_1.xml"));
    preset.print();


    for( auto button : getButtons() )
    {
        button->setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(button);
    }


}

Preset PresetManagerComponent::loadPresetFromXML(juce::File xmlFile)
{
    if(xmlFile.existsAsFile())
    {
        Preset preset;
        // parsing the xml file 
        auto element = juce::XmlDocument::parse(xmlFile); // same as creating an object and calling getDocumentElement()
        if(element->hasTagName("PRESET"))
        {
            /*
            * There, the goal is to fetch every data from every node.
            * Our nodes are:
            *  - INFORMATION
            *  - PARAMETERS
            *      - DELAY 
            *      - REVERB 
            *      - PLUGIN 
            */
            for(auto* e : element->getChildIterator())
            {
                if (e->hasTagName("INFORMATION"))
                {
                    preset.presetName = e->getStringAttribute("Name");
                    preset.authorName = e->getStringAttribute("Author");
                    preset.category   = e->getStringAttribute("Category");
                }
                else if (e->hasTagName("PARAMETERS")) 
                {
                    for(auto* child : e->getChildIterator())
                    {
                        if(child->hasTagName("DELAY"))
                        {
                            preset.delayTime            = child->getIntAttribute("Time");
                            preset.delayFeedback        = (float)child->getDoubleAttribute("Feedback");
                            preset.delaySyncToggleState = child->getBoolAttribute("SyncToggle");
                            preset.delaySyncDivider     = child->getIntAttribute("SyncDivider");
                        }
                        else if (child->hasTagName("REVERB")) 
                        {
                            preset.reverbDamping        = (float)child->getDoubleAttribute("Damping");
                            preset.reverbFreezeState    = child->getBoolAttribute("Freeze");
                            preset.reverbRoomSize       = (float)child->getDoubleAttribute("RoomSize");
                            preset.reverbWet            = (float)child->getDoubleAttribute("Wet");
                            preset.reverbDry            = (float)child->getDoubleAttribute("Dry");
                            preset.reverbWidth          = (float)child->getDoubleAttribute("Width");
                        }
                        else if (child->hasTagName("PLUGIN"))
                        {
                            preset.pluginDryWet         = (float)child->getDoubleAttribute("DryWet");
                            preset.pluginLevel          = (float)child->getDoubleAttribute("Level");
                            preset.pluginGain           = (float)child->getDoubleAttribute("Gain");
                        }
                    }
                }
            }
        }
        return preset;
    }
    return {};
}

std::vector<juce::TextButton*> PresetManagerComponent::getButtons()
{
    return 
    {
        &mPreviousButton,
        &mNextButton,
        &mDeleteButton,
        &mSaveButton
    };
}
