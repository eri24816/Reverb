/*
  ==============================================================================

    LateReverbProcessor.cpp
    Created: 6 Dec 2021 9:51:44pm
    Author:  eri24816

  ==============================================================================
*/

#include "LateReverbProcessor.h"
LateReverbProcessor::LateReverbProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
    , allpass(new float[] {0.8, 0.8}, new float[] {1, 1})
{
}

LateReverbProcessor::~LateReverbProcessor()
{
}

//==============================================================================
const juce::String LateReverbProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LateReverbProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool LateReverbProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool LateReverbProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double LateReverbProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LateReverbProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LateReverbProcessor::getCurrentProgram()
{
    return 0;
}

void LateReverbProcessor::setCurrentProgram(int index)
{
}

const juce::String LateReverbProcessor::getProgramName(int index)
{
    return {};
}

void LateReverbProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void LateReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    //auto buffer = chooseFile();

    conv.reset();

    /*
    conv.loadImpulseResponse(buffer, (double)44100, juce::dsp::Convolution::Stereo::yes
        , juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::yes);*/

    conv.loadImpulseResponse(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userMusicDirectory).getChildFile("WIDE_HALL_trim.wav"),
        juce::dsp::Convolution::Stereo::yes
        , juce::dsp::Convolution::Trim::yes, 0);
    conv.prepare(spec);

}

void LateReverbProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LateReverbProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void LateReverbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());




    int numSamples = buffer.getNumSamples();
    float systemInput[2];// 2 channels
    for (int sample = 0; sample < numSamples; ++sample) {

        // Collect input data for the IIR system
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            systemInput[channel] = *buffer.getWritePointer(channel, sample);
            if (addInpulseNextSample) {
                systemInput[channel] += 1;

            }
        }
        addInpulseNextSample = false;
        // Update the system
        float* systemOutput = reverb.update(systemInput);
        //float* systemOutput = comb.update(systemInput);
        //float* systemOutput = systemInput;

        // Write the system's output back to the AudioBuffer
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            *buffer.getWritePointer(channel, sample) = systemOutput[channel];
        }

    }

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    conv.process(context);
}

//==============================================================================
bool LateReverbProcessor::hasEditor() const
{
    return false;
}

juce::AudioProcessorEditor* LateReverbProcessor::createEditor()
{
    return false;
}

//==============================================================================
void LateReverbProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LateReverbProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
