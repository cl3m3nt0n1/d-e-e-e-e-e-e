#include "PresetManagerComponent.hpp"
#include "juce_core/juce_core.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_data_structures/juce_data_structures.h"
#include "juce_gui_basics/juce_gui_basics.h"

PresetManagerComponent::PresetManagerComponent(juce::AudioProcessorValueTreeState& valueTree) :
    apvts(valueTree)
{
    checkIfPresetsFolderPathExistsAndLoadPresets();

    mPreviousButton.setButtonText(" Prev. ");
    mNextButton.setButtonText(" Next. ");
    mSaveButton.setButtonText(" Save. ");
    mDeleteButton.setButtonText(" Del. ");

    mComboBox.setTextWhenNothingSelected("");
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
        mCurrentPresetIndex = mComboBox.getSelectedId();
        updateAPVTS(mPresetsArray[mCurrentPresetIndex - 1]);
    };

    mSaveButton.onClick = [this]()
    {
        
        // Taken out of JUCE's DialogsDemo.h
        asyncAlertWindow = std::make_unique<juce::AlertWindow> ("Save Preset",
                                                                "Enter the name of the preset to save.",
                                                                juce::MessageBoxIconType::QuestionIcon);

        asyncAlertWindow->addTextEditor ("presetName", "Name");
        asyncAlertWindow->addButton ("OK",     1, juce::KeyPress (juce::KeyPress::returnKey, 0, 0));
        asyncAlertWindow->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey, 0, 0));
        asyncAlertWindow->setEscapeKeyCancels(true);
        
        // This is clearly black magic
        asyncAlertWindow->enterModalState(true, juce::ModalCallbackFunction::create (AsyncAlertBoxResultChosen{*this}));
    };
}

void PresetManagerComponent::paint (juce::Graphics& g) 
{
}

void PresetManagerComponent::resized() 
{
    auto area = getLocalBounds();

    area.removeFromRight(getLocalBounds().getWidth() / 24);
    area.removeFromLeft(getLocalBounds().getWidth() / 24);

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.alignContent = juce::Grid::AlignContent::spaceEvenly;
    grid.alignItems = juce::Grid::AlignItems::center;

    grid.columnGap = juce::Grid::Px(5);

    grid.templateRows = {
        Track (Fr (1))
    };

    grid.templateColumns = {
        Track (Fr (1)),
        Track (Fr (1)),
        Track (Fr (6)),
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


    grid.performLayout (area);
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

void PresetManagerComponent::savePresetToXML(juce::StringRef presetName)
{
    auto preset = dumpAPVTSstate(presetName);

    // Preset preset;

    juce::XmlElement motherNode ("PRESET");
    juce::XmlElement* infoChild = new juce::XmlElement ("INFORMATION");
    
    infoChild->setAttribute("Name",     preset.presetName);
    infoChild->setAttribute("Author",   preset.authorName);
    infoChild->setAttribute("Category", preset.category);
   
    motherNode.addChildElement (infoChild);

    juce::XmlElement* parameterChild   = new juce::XmlElement ("PARAMETERS");
    juce::XmlElement* delayGrandChild  = new juce::XmlElement ("DELAY");
    juce::XmlElement* reverbGrandChild = new juce::XmlElement ("REVERB");
    juce::XmlElement* pluginGrandChild = new juce::XmlElement ("PLUGIN");

    delayGrandChild->setAttribute("Time",        preset.delayTime);
    delayGrandChild->setAttribute("Feedback",    preset.delayFeedback);
    delayGrandChild->setAttribute("SyncToggle",  preset.delaySyncToggleState ? "true" : "false");
    delayGrandChild->setAttribute("SyncDivider", preset.delaySyncDivider);

    reverbGrandChild->setAttribute("Damping",    preset.reverbDamping);
    reverbGrandChild->setAttribute("Freeze",     preset.reverbFreezeState ? "true" : "false");
    reverbGrandChild->setAttribute("RoomSize",   preset.reverbRoomSize);
    reverbGrandChild->setAttribute("Wet",        preset.reverbWet);
    reverbGrandChild->setAttribute("Dry",        preset.reverbDry);
    reverbGrandChild->setAttribute("Width",      preset.reverbWidth);

    pluginGrandChild->setAttribute("DryWet",     preset.pluginDryWet);
    pluginGrandChild->setAttribute("Level",      preset.pluginLevel);
    pluginGrandChild->setAttribute("Gain",       preset.pluginGain);

    parameterChild->addChildElement(delayGrandChild);
    parameterChild->addChildElement(reverbGrandChild);
    parameterChild->addChildElement(pluginGrandChild);

    motherNode.addChildElement(parameterChild);

    auto xmlString = motherNode.toString();

    motherNode.writeTo(Utils::PLUGIN_PRESET_PATH.getChildFile(preset.presetName.removeCharacters(" ") + ".xml"));
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
        for (int i = 0; i < mFileArray.size(); ++i) 
        {
            mPresetsArray.add(loadPresetFromXML(mFileArray[i]));
            mPresetsNameArray.add(mPresetsArray[i].presetName);
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

    if (newState.isValid())
    {
        apvts.replaceState(newState);
    }
}

Preset PresetManagerComponent::dumpAPVTSstate(juce::StringRef presetName) 
{
    Preset preset;

    preset.presetName = presetName;
    preset.authorName = "User";
    preset.category   = "USER";

    preset.delayFeedback        = apvts.getParameter("Delay Feedback")->getValue();
    preset.delayTime            = apvts.getParameterAsValue("Delay Time").getValue();
    preset.delaySyncToggleState = static_cast<bool>(apvts.getParameter("Delay Sync Toggle")->getValue());
    preset.delaySyncDivider     = apvts.getParameterAsValue("Delay Sync").getValue();

    preset.pluginLevel          = apvts.getParameter("Output Level")->getValue();
    preset.pluginGain           = apvts.getParameter("Output Gain")->getValue();
    preset.pluginDryWet         = apvts.getParameter("Plugin Dry Wet")->getValue();
    
    preset.reverbDamping        = apvts.getParameter("Reverb Damping")->getValue();
    preset.reverbDry            = apvts.getParameter("Reverb Dry")->getValue();
    preset.reverbFreezeState    = static_cast<bool>(apvts.getParameter("Reverb Freeze")->getValue());
    preset.reverbRoomSize       = apvts.getParameter("Reverb Room Size")->getValue();
    preset.reverbWet            = apvts.getParameter("Reverb Wet")->getValue();
    preset.reverbWidth          = apvts.getParameter("Reverb Width")->getValue();

    return preset;

}
