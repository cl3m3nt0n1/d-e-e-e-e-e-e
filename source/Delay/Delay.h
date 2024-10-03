//
// Created by clementone on 26/09/24.
//

#ifndef DEEEEEE_DELAY_H
#define DEEEEEE_DELAY_H

#include <juce_events/juce_events.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * @brief a simple delay class to begin with.
 * */
class Delay
{
public:
    Delay(juce::AudioProcessorValueTreeState& valueTree);

    ~Delay();

    /**
     * @brief to be called within the AudioProcessor::processBlock method.
     *        Main function of the class.
     * */
    void ProcessBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

    void PrepareToPlay(double sampleRate, int samplesPerBlock);

    void AppendToParameterLayout (juce::AudioProcessorValueTreeState::ParameterLayout& layout);

private:

    juce::AudioBuffer<float> delayBuffer;

    juce::AudioProcessorValueTreeState& apvts;


};

#endif //DEEEEEE_DELAY_H
