#include "PresetManagerComponent.hpp"



PresetManagerComponent::PresetManagerComponent()
{
    mPreviousButton.setButtonText("Previous");
    mNextButton.setButtonText("Next");
    mSaveButton.setButtonText("Save");
    mDeleteButton.setButtonText("Delete");

    mComboBox.setTextWhenNothingSelected("Select preset");

    // if our plugin's folder doesn't exist on the system, 
    if(!Utils::PLUGIN_PRESET_PATH.exists()) // Presets
    {
        DBG("Presets folder doesn't exists");
        // We create every subdirectory if they don't exist
        if(!Utils::PLUGIN_PRESET_PATH.getParentDirectory().exists()) // JucePlugin_Name
        {
            DBG(JucePlugin_Name " folder doesn't exists");

            if(!Utils::PLUGIN_PRESET_PATH.getParentDirectory().getParentDirectory().exists()) // JucePlugin_Manufacturer
            {
                DBG(JucePlugin_Manufacturer " folder doesn't exists");
                Utils::PLUGIN_PRESET_PATH.getParentDirectory().getParentDirectory().createDirectory(); 
            }
            Utils::PLUGIN_PRESET_PATH.getParentDirectory().createDirectory();
        } 
        Utils::PLUGIN_PRESET_PATH.createDirectory();
    }
    else // Our local presets directory exists  
    {
        mFileArray = Utils::PLUGIN_PRESET_PATH.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.xml");        
    }

    auto preset = loadPresetFromXML(mFileArray.getFirst());
    
    #if DEBUG
    preset.print();
    #endif

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
