#ifndef DELAY_HPP
#define DELAY_HPP

#include <juce_audio_basics/juce_audio_basics.h>

class Delay
{
public:
    Delay() {}

    void setDelayTime(int timeInMilliseconds);

    void setSampleRate(int rate);

    void setFeedbackGain(float gain);

    void prepare(int numInputChannels, int sampleRate, int samplesPerBlock);

    void process(juce::AudioBuffer<float>& buffer);

    void fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData);

    void getFromDelayBuffer (juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData);
    
    void feedbackDelay (int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer);

    juce::AudioBuffer<float>& getDelayBuffer();

    int mWritePosition = 0;

private:
    juce::AudioBuffer<float> mDelayBuffer;
    
    int mSampleRate = 44100;
    int mDelayTime = 1000;
    float mFeedbackGain = 0.5;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};

#endif