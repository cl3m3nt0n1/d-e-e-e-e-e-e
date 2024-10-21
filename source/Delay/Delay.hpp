#ifndef DELAY_HPP
#define DELAY_HPP

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
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
    void prepare(int numInputChannels, double sampleRate, int samplesPerBlock);

    /**
     * @brief To be called inside the PluginProcessor::prepareToPlay method
     * 
     * @param specs the juce::dsp::ProcessSpec related to this processor.
     */
    void prepare(const juce::dsp::ProcessSpec& specs);

    /**
     * @brief To be called within PluginProcessor::processBlock method
     *        /!\ (STILL NOT WORKING YET) /!\ 
     */
    void process(juce::dsp::ProcessContextReplacing<float>& context);

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
    void feedbackDelay (int channel, const int bufferLength, const float* dryBuffer);

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

    /**
     * @brief sets the tempo for the delay. Used within the context of sync.
     * 
     * @param BPM the current tempo of the song gotten from the DAW.
     */
    void setBPM(int BPM) {mCurrentBPM = BPM; }

    int mWritePosition = 0;


private:
    juce::AudioBuffer<float> mDelayBuffer, tempBuffer;
    juce::AudioProcessorValueTreeState& apvts;

    juce::AudioParameterInt* mDelayTimeParameter = nullptr;
    juce::AudioParameterFloat* mDelayFeedbackParameter = nullptr;
    juce::AudioParameterBool* mDelaySyncToggleParameter = nullptr;
    juce::AudioParameterChoice* mDelaySyncParameter = nullptr;

    juce::dsp::IIR::Filter<float> filter;

    
    juce::Array<int> mDelaySyncChoicesLUT = {16, 12,
                                             8,  6,  4,
                                             3,  2,  1};
    
    int mSampleRate = 44100;
    int mCurrentBPM = 120; // is set by setBPM within PluginProcessor::processBlock()

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};

#endif