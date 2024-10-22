#pragma once

#include "Delay/Delay.hpp"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_dsp/juce_dsp.h"
#include <juce_audio_processors/juce_audio_processors.h>

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

    juce::AudioProcessorValueTreeState& getApvts() { return apvts; }
private:
    /*======================== FUNCTIONS ===========================*/
    /**
     * @brief Create a Parameter Layout object. 
     *        Used when the apvts is created.
     * 
     * @return juce::AudioProcessorValueTreeState::ParameterLayout 
     */
    juce::AudioProcessorValueTreeState::ParameterLayout CreateParameterLayout();
    
    /**
     * @brief Used by the reverb to dump the values of the various
     *        reverb's related paramters from the APVTS and assign
     *        them to their respective member variables. 
     *
     * @return juce::dsp::Reverb::Parameters 
     */
    juce::dsp::Reverb::Parameters dumpParametersFromAPVTS();

    /*======================== MEMBERS ===========================*/
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", CreateParameterLayout() };
    
    Delay delay;
    
    // Reverb from juce::dsp and related parameters
    juce::dsp::Reverb reverb;
    juce::AudioParameterFloat* ReverbDampingParameter = nullptr;
    juce::AudioParameterBool* ReverbFreezeParameter = nullptr;
    juce::AudioParameterFloat* ReverbRoomSizeParameter = nullptr;
    juce::AudioParameterFloat* ReverbWetParameter = nullptr;
    juce::AudioParameterFloat* ReverbDryParameter = nullptr;
    juce::AudioParameterFloat* ReverbWidthParameter = nullptr;

    // juce::dsp drywet mixer and related audio parameter
    juce::dsp::DryWetMixer<float> dryWet;
    // juce::dsp::DryWetMixer<float> outputLevel;
    juce::dsp::Gain<float> outputLevel;
    juce::AudioParameterFloat* mPluginDryWetParameter = nullptr;    
    juce::AudioParameterFloat* mOutputLevelParameter = nullptr;    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
