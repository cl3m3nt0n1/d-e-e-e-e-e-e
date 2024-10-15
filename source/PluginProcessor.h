#pragma once

#include "Delay/Delay.hpp"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_dsp/juce_dsp.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>

#if (MSVC)
    #include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
private:
    /*======================== FUNCTIONS ===========================*/
    juce::AudioProcessorValueTreeState::ParameterLayout CreateParameterLayout();
    juce::dsp::Reverb::Parameters dumpParametersFromAPVTS();

    /*======================== MEMBERS ===========================*/
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", CreateParameterLayout() };
    
    Delay delay;
    
    
    
    juce::dsp::Reverb reverb;
    juce::AudioParameterFloat* ReverbDampingParameter = nullptr;
    juce::AudioParameterBool* ReverbFreezeParameter = nullptr;
    juce::AudioParameterFloat* ReverbRoomSizeParameter = nullptr;
    juce::AudioParameterFloat* ReverbWetParameter = nullptr;
    juce::AudioParameterFloat* ReverbDryParameter = nullptr;
    juce::AudioParameterFloat* ReverbWidthParameter = nullptr;

    juce::dsp::DryWetMixer<float> dryWet;
    juce::AudioParameterFloat* mPluginDryWetParameter = nullptr;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
