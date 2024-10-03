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

    CircularBuffer() : buffer(2, 256) {}

    /**
     *  Default constructor
     */
    CircularBuffer(int numberOfChannels, int numberOfSamples) : buffer(numberOfChannels, numberOfSamples) {}

    // ~CircularBuffer();

    /**
     * @brief prints out the values of the buffer in cout.
     */
    void PrintBuffer()
    {
        DBG("------- Circular Buffer ------");
        for(int i = 0; i < buffer.getNumChannels(); ++i)
        {
            DBG("------- New Channel ------");
            for(int j = 0; j < buffer.getNumSamples(); j++)
            {
                DBG(buffer.getSample(i, j));
            }
        }
    }

    /**
     * @brief sets a new value 'value' at index 'index'.
     * @param value the new value of the data at 'index'.
     * @param index the index of the data to be set.
     */
    void SetSameValueAtIndexForEveryChannel(T value, int index)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            SetValueAtIndexForChannel(value, index, channel);
        }
    }

    /**
     * @brief sets a new value 'value' at index 'index' for channel 'channel'.
     *        Useful to set a value for different channels.
     * @param value the new value of the data at 'index'.
     * @param index the index of the data to be set.
     */
    void SetValueAtIndexForChannel(T value, int index, int channel)
    {
        // First Case: value is in range
        if (index < buffer.getNumSamples() && index >= 0)
        {
            buffer.setSample(channel, index, value);
        }

        // Second Case: value is out of range (superior than buffer's size)
        else if (index > buffer.getNumSamples())
        {
            buffer.setSample(channel, buffer.getNumSamples() - index, value);
        }

        // Third Case: value is out of range (inferior than zero)
        else
        {
            buffer.setSample(channel,( buffer.getNumSamples() + 1) + index, value);
        }
    }




private:
    // The buffer that wil be used to store our values
    juce::AudioBuffer<T> buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularBuffer)
};


#endif //CIRCULARBUFFER_H
