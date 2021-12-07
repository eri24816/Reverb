/*
  ==============================================================================

    LateReverbEditor.h
    Created: 7 Dec 2021 2:12:19pm
    Author:  eri24816

  ==============================================================================
*/

#pragma once
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LateReverbProcessor.h"
#include "PluginProcessor.h"

class RotarySlider : public juce::Slider {
private:
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

public:
    juce::Label nameLabel;

    RotarySlider(char* name);
    RotarySlider() {};
    void resized() override;
};

//==============================================================================
/**
*/
class LateReverbEditor : public juce::Component
{
public:
    LateReverbEditor(LateReverbProcessor&);
    ~LateReverbEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    RotarySlider channelSelector, dryWet;

    RotarySlider roomSize, roomShape, decay, damping, modulationDepth;

    juce::TextButton impulseButton;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LateReverbProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LateReverbEditor)
};
