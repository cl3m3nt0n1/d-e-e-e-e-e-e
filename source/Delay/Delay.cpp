#include "Delay.hpp"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include <memory>

void Delay::setSampleRate(int rate)
{
    mSampleRate = rate;
}

void Delay::prepare(int numInputChannels, int sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
    const auto bufferSize = 2 * (sampleRate + samplesPerBlock);

    mDelayBuffer.setSize(numInputChannels, bufferSize);
    mDelayBuffer.clear();
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
    const auto delayBufferData = mDelayBuffer.getReadPointer(channel);

    auto delayTime = static_cast<int>(*apvts.getRawParameterValue("Delay Time"));

    // The index from where we want to read data
    // mSampleRate * (delayTime / 1000) -> conversion to an index of the delayTime in milliseconds
    const int readPosition = static_cast<int> (delayBufferLength + mWritePosition - (mSampleRate * delayTime / 1000)) % delayBufferLength;

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

    auto& feedbackGain = *apvts.getRawParameterValue("Delay Feedback");

    if (bufferLength + mWritePosition < delayBufferLength)
    {
        mDelayBuffer.addFromWithRamp (channel, mWritePosition, dryBuffer, bufferLength, feedbackGain, feedbackGain);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - mWritePosition;

        mDelayBuffer.addFromWithRamp(channel, bufferRemaining, dryBuffer, bufferRemaining, feedbackGain, feedbackGain);
        mDelayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, feedbackGain, feedbackGain);
    }
}

juce::AudioBuffer<float>& Delay::getDelayBuffer()
{
    return mDelayBuffer;
}

void Delay::AppendToParameterLayout (juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Delay Feedback", "Delay Feedback",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.1f, 1.f),0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterInt>("Delay Time", "Delay Time", 1, 2000, 500));
}