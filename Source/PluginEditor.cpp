/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "LateReverbProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor(ReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),lateReverbEditor(p.lateReverbProcessor)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 300);
    addAndMakeVisible(lateReverbEditor);
    
}

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor()
{
}

//==============================================================================
void ReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void ReverbAudioProcessorEditor::resized()
{
    lateReverbEditor.setBounds(getBounds());
}