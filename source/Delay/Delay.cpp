#include "Delay.hpp"
#include "juce_audio_basics/juce_audio_basics.h"

void Delay::setDelayTime(int timeInMilliseconds)
{
    mDelayTime = timeInMilliseconds;
}

void Delay::setSampleRate(int rate)
{
    mSampleRate = rate;
}

void Delay::setFeedbackGain(float gain)
{
    mFeedbackGain = gain;
}

void Delay::prepare(int numInputChannels, int sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
    const auto bufferSize = 2 * (sampleRate + samplesPerBlock);

    mDelayBuffer.setSize(numInputChannels, bufferSize);
    mDelayBuffer.clear();
}

void Delay::fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData)
{
    // read the values from buffer and store them in delayBuffer.
    if (mWritePosition + bufferLength < delayBufferLength) // if we're in range
    {
        mDelayBuffer.copyFromWithRamp (channel, mWritePosition, bufferData, bufferLength, 0.5, 0.5);
    }
    else // in this case, some values remain uncopied
    {
        const auto bufferRemaining = delayBufferLength - mWritePosition;

        mDelayBuffer.copyFromWithRamp (channel, mWritePosition, bufferData, bufferRemaining, 0.5, 0.5);
        mDelayBuffer.copyFromWithRamp (channel, 0, bufferData + bufferRemaining, bufferLength - bufferRemaining, 0.5, 0.5);
    }
}

void Delay::getFromDelayBuffer (juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData)
{
    // The index from where we want to read data
    // mSampleRate * (delayTime / 1000) -> conversion to an index of the delayTime in milliseconds
    const int readPosition = static_cast<int> (delayBufferLength + mWritePosition - (mSampleRate * mDelayTime / 1000)) % delayBufferLength;

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

void Delay::feedbackDelay (int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer)
{
    if (bufferLength + mWritePosition < delayBufferLength)
    {
        mDelayBuffer.addFromWithRamp (channel, mWritePosition, dryBuffer, bufferLength, 0.5, 0.5);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - mWritePosition;

        mDelayBuffer.addFromWithRamp(channel, bufferRemaining, dryBuffer, bufferRemaining, 0.5, 0.5);
        mDelayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, 0.5, 0.5);
    }
}

juce::AudioBuffer<float>& Delay::getDelayBuffer()
{
    return mDelayBuffer;
}