#include "PluginProcessor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
#if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
    #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ) /* , delayLine(44100 * 2)*/
{
    /*     delayLine.setDelayTime(30000);
    delayLine.setFeedbackGain(0.5); */
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    juce::ignoreUnused (sampleRate);
    // delayLine.prepare(sampleRate);

    // From AudioProgrammer tutorial
    const int numInputChannels = getTotalNumInputChannels();
    mSampleRate = static_cast<int> (sampleRate);
    const int bufferSize = 2 * (mSampleRate + samplesPerBlock);

    mDelayBuffer.setSize (numInputChannels, bufferSize);
    mDelayBuffer.clear();
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
    #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
#endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const auto bufferLength = buffer.getNumSamples();
    const auto delayBufferLength = mDelayBuffer.getNumSamples();


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const auto* bufferData = buffer.getReadPointer (channel);
        const auto* delayBufferData = mDelayBuffer.getReadPointer (channel);
        auto dryBuffer = buffer.getWritePointer(channel);
        // read the values from buffer and store them in delayBuffer.
        fillDelayBuffer (channel, bufferLength, delayBufferLength, bufferData);

        // read the values from the delayBuffer and write them to the audio buffer.
        getFromDelayBuffer (buffer, channel, bufferLength, delayBufferLength, delayBufferData);

        feedbackDelay(channel, bufferLength, delayBufferLength, dryBuffer);
        
    }
    /* We read bufferLength values so we need to increment the write position
     * so that, next time, we read once again bufferLength values but don't 
     * overwrite the previously saved values.
     */
    mWritePosition += bufferLength;
    mWritePosition %= delayBufferLength; // wrap around
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    //    return new PluginEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::CreateParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // delay.AppendToParameterLayout (layout);

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

void PluginProcessor::fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData)
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

void PluginProcessor::getFromDelayBuffer (juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData)
{
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

void PluginProcessor::feedbackDelay (int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer)
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