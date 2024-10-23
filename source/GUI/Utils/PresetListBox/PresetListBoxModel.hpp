#pragma once

#include "juce_core/juce_core.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class PresetListBoxModel : public juce::TableListBoxModel
{
public:
    PresetListBoxModel();

    /**
     * @brief an override from TableListBoxModel
     */
    int getNumRows() override
    {

    }

    /**
     * @brief an override from TableListBoxModel
     */
    void paintRowBackground (juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected) override
    {

    }

    /**
     * @brief an override from TableListBoxModel
     */
    void paintCell (juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {

    }

private:
    std::unique_ptr<juce::XmlElement> mPreset;
    juce::XmlElement* mInfoList = nullptr;
    juce::XmlElement* mParameterList = nullptr;

};