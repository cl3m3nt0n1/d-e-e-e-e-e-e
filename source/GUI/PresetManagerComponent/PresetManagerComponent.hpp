#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include "../Utils/Utils.hpp"

/**
 * @brief a simple struct to contain every parameter and 
 *        values of a preset dumped from an XML file. 
 */
struct Preset 
{
    // PRESET INFO
    juce::String presetName    = {};
    juce::String authorName    = {};
    juce::String category      = {};

    // PARAMETERS VALUES
    //    DELAY 
    int   delayTime            = 0;
    float delayFeedback        = 0.0f;
    bool  delaySyncToggleState = false;
    int   delaySyncDivider     = 1;

    //    REVERB
    float reverbDamping        = 0.0f;
    bool  reverbFreezeState    = false;
    float reverbRoomSize       = 0.0f;
    float reverbWet            = 0.0f;
    float reverbDry            = 0.0f;
    float reverbWidth          = 0.0f;

    //    PLUGIN
    float pluginDryWet         = 0.0f;
    float pluginLevel          = 0.0f;
    float pluginGain           = 0.0f;

    #if DEBUG
    /* ========= MEMBERS ========== */
    void print()
    {
        // DBG PURPOSES ONLY
        DBG("presetName:");        DBG(presetName);
        DBG("authorName:");        DBG(authorName);
        DBG("category:");          DBG(category);
        DBG("delayTime:");         DBG(delayTime);
        DBG("reverbDamping:");     DBG(reverbDamping);
        DBG("pluginDryWet:");      DBG(pluginDryWet);
    }
    #endif
};


class PresetManagerComponent : public juce::Component
{
public:
    PresetManagerComponent(juce::AudioProcessorValueTreeState& valueTree);

    void paint (juce::Graphics& g) override;

    void resized() override;
    
private:

    

    /* =================== MEMBERS ======================= */
    juce::TextButton mPreviousButton, mNextButton, mSaveButton, mDeleteButton;
    juce::ComboBox mComboBox; // Right now, only a combo box. TODO: Replace with PrestListBoxComponent
    juce::Array<juce::File> mFileArray = {};
    juce::Array<Preset> mPresetsArray = {};
    juce::StringArray mPresetsNameArray = {};
    int mCurrentPresetIndex = 0;
    juce::AudioProcessorValueTreeState& apvts;

    std::unique_ptr<juce::AlertWindow> asyncAlertWindow;

    /* =================== METHODS ======================= */
    
    /**
     * @brief just a handy helper function. Returns a vector of
     *        references to the various TextButton of this component.
     * 
     * @return std::vector<juce::TextButton*> 
     */
    std::vector<juce::TextButton*> getButtons();
    
    /**
     * @brief checks if the complete preset path exists 
     *        and if it doesn't, creates it and loads 
     *        its content.
     */
    void checkIfPresetsFolderPathExistsAndLoadPresets();
    
    /**
     * @brief Dump the content of the xml file 'xmlFile'
     *        and returns it in a Preset structure form. 
     * 
     * @param xmlFile the juce::File corresponding to the preset stored as xml.
     * @return Preset 
     */
    Preset loadPresetFromXML(juce::File xmlFile);
    
    /**
     * @brief dumps the state of the apvts and saves it as an XML file.
     */
    void savePresetToXML(juce::StringRef presetName);

    /**
     * @brief Updates the state of the current apvts (set previously by reference)
     *        using the preset param values. This internally uses a juce::ValueTree
     *        and the AudioProcessorValueTreeState::replaceState() method. 
     * @param preset The set of values that represent a preset.
     */
    void updateAPVTS(Preset preset);


    /* ============= STRUCTS ============== */
    
    /**
     * @brief A struct to handle black magic. 
     *        This is called when the user clicks on "Save".
     *        Manages the end of life of the asyncAlertWindow
     *        and calls for savePresetToXML().
     */
    struct AsyncAlertBoxResultChosen
    {
        
        AsyncAlertBoxResultChosen(PresetManagerComponent& presetManager) : manager(presetManager) {  }

        void operator() (int result) const noexcept
        {
            manager.asyncAlertWindow->exitModalState (result);
            manager.asyncAlertWindow->setVisible (false);
            manager.savePresetToXML(manager.asyncAlertWindow->getTextEditorContents("presetName"));
            manager.asyncAlertWindow.reset(); // deleting the pointed AlertWindow
        }

        private: 
        PresetManagerComponent& manager;
    };




JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManagerComponent)
};