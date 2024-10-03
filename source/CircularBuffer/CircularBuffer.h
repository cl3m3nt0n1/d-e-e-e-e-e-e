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
    void PrintBuffer();

    /**
     * @brief sets a new value 'value' at index 'index'.
     * @param value the new value of the data at 'index'.
     * @param index the index of the data to be set.
     */
    void SetSameValueAtIndexForEveryChannel(T value, int index);

    /**
     * @brief sets a new value 'value' at index 'index' for channel 'channel'.
     *        Useful to set a value for different channels.
     * @param value the new value of the data at 'index'.
     * @param index the index of the data to be set.
     */
    void SetValueAtIndexForChannel(T value, int index, int channel);




private:
    // The buffer that wil be used to store our values
    juce::AudioBuffer<T> buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularBuffer)
};


#endif //CIRCULARBUFFER_H
