//
// Created by clementone on 02/10/24.
//

#include "CircularBuffer.h"


// WHY C++? 
// WHY??
template class CircularBuffer<float>;
template void CircularBuffer<float>::SetValueAtIndexForChannel(float value, int index, int channel);
template void CircularBuffer<float>::SetSameValueAtIndexForEveryChannel(float value, int index);
template void CircularBuffer<float>::PrintBuffer();

template <typename T>
void CircularBuffer<T>::SetValueAtIndexForChannel(T value, int index, int channel)
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

template <typename T>
void CircularBuffer<T>::SetSameValueAtIndexForEveryChannel(T value, int index)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        SetValueAtIndexForChannel(value, index, channel);
    }
}

template <typename T>
void CircularBuffer<T>::PrintBuffer()
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

