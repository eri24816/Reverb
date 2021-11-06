/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


RotarySlider::RotarySlider(ReverbAudioProcessor& audioProcessor, char* varName) :nameLabel("nameLabel", varName) {
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    setTextBoxStyle(juce::Slider::TextBoxBelow,
        true,
        getTextBoxWidth(),
        getTextBoxHeight());

    setRotaryParameters(-2.5 + 6.283, 2.5 + 6.283, true);
    setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    nameLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(nameLabel);
};

void RotarySlider::resized() {
    juce::Slider::resized();
    nameLabel.setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(0, .8, 1, .2)));
}

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor(ReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), R(audioProcessor, "APF r"), theta(audioProcessor, "APF freq")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible(R);
    theta.setRange(0,3.14159,0.1);
    R.setRange(0, 1,0.01);
    theta.onValueChange= R.onValueChange = [this]() {
        float R = this->R.getValue();
        float theta = this->theta.getValue();
        //this->audioProcessor.reverb.allpass.R2 = R*R; 
        //this->audioProcessor.reverb.allpass.twoRCosTheta= 2*R*cos(theta);

    };
    addAndMakeVisible(theta);
    addAndMakeVisible(inpulseButton);
    inpulseButton.onClick = [&p]() {p.addInpulse();};
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
    juce::FlexBox flexbox;
    flexbox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexbox.justifyContent = juce::FlexBox::JustifyContent::center;
    flexbox.alignContent = juce::FlexBox::AlignContent::center;
    flexbox.items = {juce::FlexItem( R).withMinWidth(70.0f).withMinHeight(90.0f),juce::FlexItem(theta).withMinWidth(70.0f).withMinHeight(90.0f) };
    flexbox.performLayout(getLocalBounds().getProportion(juce::Rectangle<float>(0, 0, 1, 0.8)).reduced(10));
    inpulseButton.setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(0, 0.5, 1, 0.2)).reduced(10));
}
