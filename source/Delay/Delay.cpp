#include "Delay.hpp"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include <memory>

Delay::Delay (juce::AudioProcessorValueTreeState& valueTree) : apvts (valueTree),
                                                               mDelayTimeParameter (dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter ("Delay Time"))),
                                                               mDelayFeedbackParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Delay Feedback"))),
                                                               mDelaySyncToggleParameter (dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter ("Delay Sync Toggle"))),
                                                               mDelaySyncParameter (dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter ("Delay Sync")))
{
}

void Delay::setSampleRate (int rate)
{
    mSampleRate = rate;
}

void Delay::prepare (int numInputChannels, int sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
    const auto bufferSize = 2 * (sampleRate + samplesPerBlock);

    mDelayBuffer.setSize (numInputChannels, bufferSize);
    mDelayBuffer.clear();
}

void Delay::process (juce::dsp::ProcessContextReplacing<float> context)
{
    auto block = context.getInputBlock();
    auto numChannels = block.getNumChannels();

    // TODO: DON'T ALLOCATE IN PROCESS!
    // THIS IS A TEMPORARY SOLUTION.
    juce::AudioBuffer<float> tempBuffer;
    const auto numSamples = block.getNumSamples();
    tempBuffer.setSize (numChannels, numSamples);
    auto bufferLength = tempBuffer.getNumSamples();

    block.copyTo (tempBuffer);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        const auto* bufferData = tempBuffer.getReadPointer (channel);
        auto dryBuffer = tempBuffer.getWritePointer (channel);
        // read the values from buffer and store them in delayBuffer.
        fillDelayBuffer (channel, bufferLength, bufferData);
        // read the values from the delayBuffer and write them to the audio buffer.
        getFromDelayBuffer (tempBuffer, channel, bufferLength);
        // apply feedback
        feedbackDelay (channel, bufferLength, dryBuffer);
    }
    /* We read bufferLength values so we need to increment the write position
     * so that, next time, we read once again bufferLength values but don't 
     * overwrite the previously saved values.
     */
    mWritePosition += bufferLength;
    mWritePosition %= mDelayBuffer.getNumSamples(); // wrap around
}

void Delay::fillDelayBuffer (int channel, const int bufferLength, const float* bufferData)
{
    const auto delayBufferLength = mDelayBuffer.getNumSamples();
    // read the values from buffer and store them in delayBuffer.
    if (mWritePosition + bufferLength < delayBufferLength) // if we're in range
    {
        mDelayBuffer.copyFromWithRamp (channel, mWritePosition, bufferData, bufferLength, 0.8f, 0.8f);
    }
    else // in this case, some values remain uncopied
    {
        const auto bufferRemaining = delayBufferLength - mWritePosition;

        mDelayBuffer.copyFromWithRamp (channel, mWritePosition, bufferData, bufferRemaining, 0.8f, 0.8f);
        mDelayBuffer.copyFromWithRamp (channel, 0, bufferData + bufferRemaining, bufferLength - bufferRemaining, 0.8f, 0.8f);
    }
}

void Delay::getFromDelayBuffer (juce::AudioBuffer<float>& buffer, int channel, const int bufferLength)
{
    const auto delayBufferLength = mDelayBuffer.getNumSamples();
    const auto delayBufferData = mDelayBuffer.getReadPointer (channel);

    auto delayTime = mDelayTimeParameter->get();
    int readPosition = 0; // The index from where we want to read data

    // mSampleRate * (delayTime / 1000) -> conversion to an index of the delayTime to a time in milliseconds
    mDelaySyncToggleParameter->get() ? // if user wants to sync to tempo
        readPosition = static_cast<int> (delayBufferLength + mWritePosition - ((mSampleRate * (1000 * 60 / mCurrentBPM) / mDelaySyncChoicesLUT[mDelaySyncParameter->getIndex()]) / 1000)) % delayBufferLength
                                     : readPosition = static_cast<int> (delayBufferLength + mWritePosition - (mSampleRate * delayTime / 1000)) % delayBufferLength;

    // if we're in range
    if (bufferLength + readPosition < delayBufferLength)
    {
        buffer.copyFrom (channel, 0, delayBufferData + readPosition, bufferLength);
    }
    else // like when we wanted to fill the delay buffer, we need to be careful not to write out of bounds.
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.copyFrom (channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.copyFrom (channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }
}

void Delay::feedbackDelay (int channel, const int bufferLength, float* dryBuffer)
{
    const auto delayBufferLength = mDelayBuffer.getNumSamples();

    auto& feedbackGain = *apvts.getRawParameterValue ("Delay Feedback");

    if (bufferLength + mWritePosition < delayBufferLength)
    {
        mDelayBuffer.addFromWithRamp (channel, mWritePosition, dryBuffer, bufferLength, feedbackGain, feedbackGain);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - mWritePosition;

        mDelayBuffer.addFromWithRamp (channel, bufferRemaining, dryBuffer, bufferRemaining, feedbackGain, feedbackGain);
        mDelayBuffer.addFromWithRamp (channel, 0, dryBuffer, bufferLength - bufferRemaining, feedbackGain, feedbackGain);
    }
}

juce::AudioBuffer<float>& Delay::getDelayBuffer()
{
    return mDelayBuffer;
}

void Delay::AppendToParameterLayout (juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Delay Feedback", "Delay Feedback", juce::NormalisableRange<float> (0.01f, 1.0f, 0.01f, 1.f), 0.1f));

    layout.add (std::make_unique<juce::AudioParameterInt> ("Delay Time", "Delay Time", 1, 2000, 500));

    layout.add (std::make_unique<juce::AudioParameterBool> ("Delay Sync Toggle", "Delay Sync Toggle", false));

    juce::StringArray choices = { "1:16", "1:12", "1:8", "1:6", "1:4", "1:3", "1:2", "1:1" };

    layout.add (std::make_unique<juce::AudioParameterChoice> ("Delay Sync", "Delay Sync", choices, choices.size() - 1));
}