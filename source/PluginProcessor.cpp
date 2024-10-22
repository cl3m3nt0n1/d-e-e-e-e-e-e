#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
#if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
    #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      delay (apvts),
      ReverbDampingParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Reverb Damping"))),
      ReverbFreezeParameter (dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter ("Reverb Freeze"))),
      ReverbRoomSizeParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Reverb Room Size"))),
      ReverbWetParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Reverb Wet"))),
      ReverbDryParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Reverb Dry"))),
      ReverbWidthParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Reverb Width"))),
      mOutputLevelParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Output Level"))),
      mOutputGainParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Output Gain"))),
      mPluginDryWetParameter (dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter ("Plugin Dry Wet")))
{
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

    // delay.prepare (getTotalNumInputChannels(), static_cast<int> (sampleRate), samplesPerBlock);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    spec.sampleRate = sampleRate;

    delay.prepare(spec);

    reverb.prepare (spec);
    reverb.reset();
    reverb.setEnabled (true);

    dryWet.prepare (spec);
    dryWet.setMixingRule (juce::dsp::DryWetMixingRule::linear);
    dryWet.reset();

    outputLevel.prepare(spec);
    outputLevel.reset();

    outputGain.prepare(spec);
    outputGain.reset();

    
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

    // Fetching the song's BPM has to be done within processBlock()
    auto bpm = getPlayHead()->getPosition()->getBpm();
    if (bpm.hasValue())
        delay.setBPM (static_cast<int> (*bpm));

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

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);


    dryWet.pushDrySamples (block);

    const auto bufferLength = buffer.getNumSamples();
    const auto delayBufferLength = delay.getDelayBuffer().getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const auto* bufferData = buffer.getReadPointer (channel);
        auto dryBuffer = buffer.getWritePointer (channel);
        // read the values from buffer and store them in delayBuffer.
        delay.fillDelayBuffer (channel, bufferLength, bufferData);
        // read the values from the delayBuffer and write them to the audio buffer.
        delay.getFromDelayBuffer (buffer, channel, bufferLength);
        // apply feedback
        delay.feedbackDelay (channel, bufferLength, dryBuffer);
    }
    /* We read bufferLength values so we need to increment the write position
     * so that, next time, we read once again bufferLength values but don't 
     * overwrite the previously saved values.
     */
    delay.mWritePosition += bufferLength;
    delay.mWritePosition %= delayBufferLength; // wrap around

    // delay.process(context);

    reverb.setParameters (dumpParametersFromAPVTS());
    reverb.process (context);

    /*  
        // TODO: Use process function
        delay.process(context); 
    */
    dryWet.mixWetSamples (block);
    dryWet.setWetMixProportion (mPluginDryWetParameter->get());

    outputLevel.setGainLinear(mOutputLevelParameter->get());
    outputLevel.process(context);

    outputGain.setGainLinear(mOutputGainParameter->get());
    outputGain.process(context);
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
    // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    /*
     * Write the state of the APVTS to a memory output stream.
     * i.e. Serialize the APVTS' state.
     * */
    juce::MemoryOutputStream outputStream (destData, true);
    apvts.state.writeToStream (outputStream);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState (tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::CreateParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Delay Parameters
    delay.AppendToParameterLayout (layout);

    // Reverb Parameters
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Reverb Damping", "Reverb Damping", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 1.f), 0.1f));

    layout.add (std::make_unique<juce::AudioParameterBool> ("Reverb Freeze", "Reverb Freeze", false));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("Reverb Room Size", "Reverb Room Size", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 1.f), 0.2f));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("Reverb Wet", "Reverb Wet", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 1.f), 0.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("Reverb Dry", "Reverb Dry", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 1.f), 0.2f));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("Reverb Width", "Reverb Width", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f, 1.f), 0.2f));

    //Plugin Parameters
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Plugin Dry Wet", "Plugin Dry Wet", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Output Level", "Output Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Output Gain", "Output Gain", juce::NormalisableRange<float> (1.0f, 1.5f, 0.01f), 1.0f));

    return layout;
}

juce::dsp::Reverb::Parameters PluginProcessor::dumpParametersFromAPVTS()
{
    juce::dsp::Reverb::Parameters tempParams;
    tempParams.damping = ReverbDampingParameter->get();
    tempParams.freezeMode = static_cast<float> (ReverbFreezeParameter->get());
    tempParams.roomSize = ReverbRoomSizeParameter->get();
    tempParams.wetLevel = ReverbWetParameter->get();
    tempParams.dryLevel = ReverbDryParameter->get();
    tempParams.width = ReverbWidthParameter->get();

    return tempParams;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
