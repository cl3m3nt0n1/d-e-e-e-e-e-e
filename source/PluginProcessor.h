#pragma once

#include "Delay/Delay.hpp"
#include "juce_audio_basics/juce_audio_basics.h"
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

    // From AudioProgrammer tutorial
    void fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData);

    void getFromDelayBuffer (juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData);
    void feedbackDelay (int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer);

private:
    /*======================== FUNCTIONS ===========================*/
    juce::AudioProcessorValueTreeState::ParameterLayout CreateParameterLayout();

    /*======================== MEMBERS ===========================*/
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", CreateParameterLayout() };
    
    // From AudioProgrammer tutorial
    juce::AudioBuffer<float> mDelayBuffer;
    int mWritePosition = 0;
    int mSampleRate = 44100;
    int delayTime = 1000;

    Delay delay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
