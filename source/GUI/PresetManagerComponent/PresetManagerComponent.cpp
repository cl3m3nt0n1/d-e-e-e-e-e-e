#include "PresetManagerComponent.hpp"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_data_structures/juce_data_structures.h"
#include "juce_gui_basics/juce_gui_basics.h"

PresetManagerComponent::PresetManagerComponent(juce::AudioProcessorValueTreeState& valueTree) :
    apvts(valueTree)
{
    checkIfPresetsFolderPathExistsAndLoadPresets();

    mPreviousButton.setButtonText("Previous");
    mNextButton.setButtonText("Next");
    mSaveButton.setButtonText("Save");
    mDeleteButton.setButtonText("Delete");

    mComboBox.setTextWhenNothingSelected("Select preset");
    mComboBox.setSelectedId(mCurrentPresetIndex);
    addAndMakeVisible(mComboBox);


    for( auto button : getButtons() )
    {
        button->setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(button);
    }

    mNextButton.onClick = [this]()
    {
        if(mCurrentPresetIndex < mPresetsNameArray.size())
        {
            mCurrentPresetIndex++;
            mComboBox.setSelectedId(mCurrentPresetIndex);
            updateAPVTS(mPresetsArray[mCurrentPresetIndex]);
        }
    };
    
    mPreviousButton.onClick = [this]()
    {
        if(mCurrentPresetIndex > 1)
        {
            mCurrentPresetIndex--;
            mComboBox.setSelectedId(mCurrentPresetIndex);
            updateAPVTS(mPresetsArray[mCurrentPresetIndex]);
        }
    };

    mComboBox.onChange = [this]()
    {
        updateAPVTS(mPresetsArray[mComboBox.getSelectedId()]);
    };

}

void PresetManagerComponent::paint (juce::Graphics& g) 
{
    
}

void PresetManagerComponent::resized() 
{
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.alignContent = juce::Grid::AlignContent::spaceEvenly;
    grid.alignItems = juce::Grid::AlignItems::center;

    grid.templateRows = {
        Track (Fr (1))
    };

    grid.templateColumns = {
        Track (Fr (1)),
        Track (Fr (1)),
        Track (Fr (10)),
        Track (Fr (1)),
        Track (Fr (1))
    };

    for (int i = 0; i < 5; ++i)
        grid.items.set (i, juce::GridItem (nullptr));

    grid.items.set (0, mPreviousButton);
    grid.items.set (1, mNextButton);
    grid.items.set (2, mComboBox);
    grid.items.set (3, mSaveButton);
    grid.items.set (4, mDeleteButton);


    grid.performLayout (getLocalBounds());
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

void PresetManagerComponent::checkIfPresetsFolderPathExistsAndLoadPresets()
{
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
        
        // Fill mPresetsArray
        for (auto file : mFileArray) 
        {
            mPresetsArray.add(loadPresetFromXML(file));
            mPresetsNameArray.add(mPresetsArray.getLast().presetName);
        }
        mComboBox.addItemList(mPresetsNameArray, 1);
    }
}

void PresetManagerComponent::updateAPVTS(Preset preset)
{
    // Not the perfect way of doing it...
    juce::ValueTree newState
    { "Parameters", {},
        {
            { "PARAM", {{ "id", "Delay Feedback"    }, { "value", preset.delayFeedback }}},
            { "PARAM", {{ "id", "Delay Sync"        }, { "value", preset.delaySyncDivider }}},
            { "PARAM", {{ "id", "Delay Sync Toggle" }, { "value", preset.delaySyncToggleState }}},
            { "PARAM", {{ "id", "Delay Time"        }, { "value", preset.delayTime }}},
            { "PARAM", {{ "id", "Output Level"      }, { "value", preset.pluginLevel }}},
            { "PARAM", {{ "id", "Plugin Dry Wet"    }, { "value", preset.pluginDryWet }}},
            { "PARAM", {{ "id", "Output Gain"       }, { "value", preset.pluginGain }}},
            { "PARAM", {{ "id", "Reverb Damping"    }, { "value", preset.reverbDamping }}},
            { "PARAM", {{ "id", "Reverb Dry"        }, { "value", preset.reverbDry }}},
            { "PARAM", {{ "id", "Reverb Freeze"     }, { "value", preset.reverbFreezeState }}},
            { "PARAM", {{ "id", "Reverb Room Size"  }, { "value", preset.reverbRoomSize }}},
            { "PARAM", {{ "id", "Reverb Wet"        }, { "value", preset.reverbWet }}},
            { "PARAM", {{ "id", "Reverb Width"      }, { "value", preset.reverbWidth }}},
        }
    };

    DBG(newState.toXmlString());

    if (newState.isValid())
    {
        DBG("Tree is valid.");
        apvts.replaceState(newState);
    }
}