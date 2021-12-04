/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "IIR.h"
//==============================================================================
/**
*/
class ReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ReverbAudioProcessor();
    ~ReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void addInpulse() {
        addInpulseNextSample = true;
    }
    IIR::Reverb<2> reverb;
    IIR::Allpass2<2> allpass;
    juce::dsp::Convolution conv;
    
    juce::AudioSampleBuffer chooseFile() {
        
        juce::File filterFile("D:\\impulse_response\\WIDE HALL-1.wav");
        juce::AudioBuffer<float> sourceBuffer;
        return sourceBuffer;
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        auto* reader = formatManager.createReaderFor(filterFile);

        sourceBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&sourceBuffer, 0, (int)reader->lengthInSamples, 0, false, false);

        int len = sourceBuffer.getNumSamples();
        std::cout << "Length: " << len << std::endl;
        for (int i = 0; i < len; i++) {
            std::cout << *(sourceBuffer.getReadPointer(0) + i) << " ";
            if (i == 100)
                break;
        }
        return sourceBuffer;
    }

private:

    int addInpulseNextSample = false;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbAudioProcessor)
};
