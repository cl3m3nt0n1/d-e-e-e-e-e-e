#ifndef DELAY_HPP
#define DELAY_HPP

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"
#include <juce_audio_basics/juce_audio_basics.h>

class Delay
{
public:
    Delay(juce::AudioProcessorValueTreeState& valueTree);

    /**
     * @brief Set the Sample Rate object
     * 
     * @param rate the current sample rate
     */
    void setSampleRate(int rate);

    /**
     * @brief To be called inside the PluginProcessor::prepareToPlay method
     * 
     * @param numInputChannels the total number of input channels
     * @param sampleRate the current sample rate
     * @param samplesPerBlock the number of samples per block of audio
     */
    void prepare(int numInputChannels, int sampleRate, int samplesPerBlock);

    /**
     * @brief To be called within PluginProcessor::processBlock method
     *        /!\ (NOT WORKING YET) /!\ 
     */
    void process(juce::dsp::ProcessContextReplacing<float> context);

    /**
     * @brief the read operation of a DDL. Reads the content of the AudioBuffer
     *        given by JUCE and copy its content to the DDL's circular buffer.
     * 
     * @param channel The current channel of the AudioBuffer to process
     * @param bufferLength The size of the Buffer given by JUCE in processBlock
     * @param bufferData A juce::AudioBuffer::GetReadPointer() pointer to the content of the main buffer, Buffer.
     */
    void fillDelayBuffer (int channel, const int bufferLength, const float* bufferData);

    /**
     * @brief the write operation of the DDL. Reads the content of the delay buffer
     *        and writes it to the main buffer given by JUCE in processBlock     * 
     * @param buffer a reference to the main buffer given in processBlock
     * @param channel the channel of buffer to process
     * @param bufferLength the length of the buffer, buffer
     */
    void getFromDelayBuffer (juce::AudioBuffer<float>& buffer, int channel, const int bufferLength);
    
    /**
     * @brief adds feedback to the DDL
     * 
     * @param channel the channel of buffer to process
     * @param bufferLength the length of the main buffer
     * @param dryBuffer a writePointer to the main buffer
     */
    void feedbackDelay (int channel, const int bufferLength, float* dryBuffer);

    /**
     * @brief Get the Delay Buffer 
     * 
     * @return juce::AudioBuffer<float>& 
     */
    juce::AudioBuffer<float>& getDelayBuffer();

    /**
     * @brief Appends the list of parameters needed by this class to the main APVTS
     * 
     * @param layout the main APVTS o the plugin
     */
    void AppendToParameterLayout (juce::AudioProcessorValueTreeState::ParameterLayout& layout);

    int mWritePosition = 0;

private:
    juce::AudioBuffer<float> mDelayBuffer;
    juce::AudioProcessorValueTreeState& apvts;
    juce::AudioParameterInt* mDelayTimeParameter = nullptr;
    juce::AudioParameterFloat* mDelayFeedbackParameter = nullptr;
    // juce::AudioParameterBool* mDelaySyncParameter = nullptr;
    
    int mSampleRate = 44100;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};

#endif