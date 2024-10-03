//
// Created by clementone on 26/09/24.
//

#include "Delay.h"
Delay::Delay (juce::AudioProcessorValueTreeState& valueTree) : apvts (valueTree)
{

}

Delay::~Delay()
{
}

void Delay::ProcessBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    delayBuffer.copyFrom (0, 1, buffer, 0, 1, buffer.getNumSamples());
//    delayBuffer.copyFrom (1, 0, buffer, 1, 0, buffer.getNumSamples());

    for(int channel = 0 ; channel < delayBuffer.getNumChannels() ; channel++)
        for (int i = 0; i < delayBuffer.getNumSamples(); i++)
        {
            auto sample = delayBuffer.getSample(channel, i);
            sample *= *apvts.getRawParameterValue ("Feedback");
        }
}

void Delay::PrepareToPlay (double sampleRate, int samplesPerBlock)
{

}

void Delay::AppendToParameterLayout (juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Delay",
        "Delay",
        juce::NormalisableRange<float> (1, 3500, 1),
            1700));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("Feedback",
        "Feedback",
        juce::NormalisableRange<float> (0, 1, 0.01f),
            0.75f));
}

