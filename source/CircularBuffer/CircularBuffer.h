//
// Created by clementone on 02/10/24.
//

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <juce_audio_basics/juce_audio_basics.h>

template <typename T>
class CircularBuffer
{
public:
    CircularBuffer() : buffer (2, 256) {}

    /**
     *  Default constructor
     */
    CircularBuffer (int numberOfChannels, int numberOfSamples) : buffer (numberOfChannels, numberOfSamples) {}

    // ~CircularBuffer();

    /**
     * @brief prints out the values of the buffer in cout.
     */
    void PrintBuffer()
    {
        DBG ("------- Circular Buffer ------");
        for (int i = 0; i < buffer.getNumChannels(); ++i)
        {
            DBG ("------- New Channel ------");
            for (int j = 0; j < buffer.getNumSamples(); j++)
            {
                DBG (buffer.getSample (i, j));
            }
        }
    }

    /**
     * @brief sets a new value 'value' at index 'index'.
     * @param value the new value of the data at 'index'.
     * @param index the index of the data to be set.
     */
    void SetSameValueAtIndexForEveryChannel (T value, int index)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            SetValueAtIndexForChannel (value, index, channel);
        }
    }

    /**
     * @brief sets a new value 'value' at index 'index' for channel 'channel'.
     *        Useful to set a value for different channels.
     * @param value the new value of the data at 'index'.
     * @param index the index of the data to be set.
     */
    void SetValueAtIndexForChannel (T value, int index, int channel)
    {
        auto totalSize = buffer.getNumSamples();
        auto lastIndex = totalSize - 1;

        if (index > lastIndex) // superior case
        {
            buffer.setSample (channel, totalSize - index, value);
            buffer[1] = 0;
        }
        else if (index < 0) // inferior case 
        {
            buffer.setSample(channel, totalSize + index, value);
        }
        else // Everything's fine
        {
         buffer.setSample(channel, index, value);
        }
    }

    /**
     * @brief A call to juce::AudioBuffer::getNumChannels()
     */
    int getNumChannels()
    {
        return buffer.getNumChannels();
    }

    /**
     * @brief A call to juce::AudioBuffer::getNumSamples()
     */
    int getNumSamples()
    {
        buffer.getNumSamples();
    }

    /**
     * @brief A call to juce::AudioBuffer::copyFrom()
     */
    void copyFrom(int destChannel, int destStartSample, const juce::AudioBuffer<T>& bufferToCopyFrom, int sourceChannel, int sourceStartSample, int numSamples)
    {
        buffer.copyFrom(destChannel, destStartSample, bufferToCopyFrom, sourceChannel, sourceStartSample, numSamples);
    }

    /**
     * @brief A call to juce::AudioBuffer::getSample()
     */
    T getSample(int channel, int index)
    {
        buffer.getSample(channel, index);
    }

private:
    // The buffer that wil be used to store our values
    juce::AudioBuffer<T> buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularBuffer)
};

#endif //CIRCULARBUFFER_H
