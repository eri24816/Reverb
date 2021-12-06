/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "LateReverbProcessor.h"
#include "PluginEditor.h"


RotarySlider::RotarySlider(ReverbAudioProcessor& audioProcessor, char* varName) :nameLabel("nameLabel", varName) {
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    setTextBoxStyle(juce::Slider::TextBoxBelow,
        true,
        getTextBoxWidth(),
        getTextBoxHeight()*2);

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
    : AudioProcessorEditor(&p), audioProcessor(p), channelSelector(audioProcessor, "channel : -1"), roomSize(audioProcessor,""),roomShape(audioProcessor, ""), decay(audioProcessor, ""), damping(audioProcessor, ""), modulationDepth(audioProcessor, ""),
    impulseButton("impulse"), dryWet(audioProcessor,"")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 300);
    addAndMakeVisible(channelSelector);
    
    LateReverbProcessor* lrp = &p.lateReverbProcessor;
    
    addAndMakeVisible(roomSize);
    roomSize.setRange(1, 5000);
    roomSize.onValueChange = [this, lrp]() {
        roomSize.nameLabel.setText("room size : " + juce::String(this->roomSize.getValue()), juce::NotificationType::dontSendNotification);
        lrp->reverb.SetRoomSize(this->roomSize.getValue());
    };
    roomSize.setValue(lrp->reverb.GetRoomSize());

    addAndMakeVisible(roomShape);
    roomShape.setRange(0,1);
    roomShape.onValueChange = [this, lrp]() {
        roomShape.nameLabel.setText("room shape : " + juce::String(this->roomShape.getValue()), juce::NotificationType::dontSendNotification);
        lrp->reverb.SetRoomShape(this->roomShape.getValue());
    };
    roomShape.setValue(lrp->reverb.GetRoomShape());

    addAndMakeVisible(decay);
    decay.setRange(0, 1);
    decay.onValueChange = [this, lrp]() {
        decay.nameLabel.setText("decay : " + juce::String(this->decay.getValue()), juce::NotificationType::dontSendNotification);
        lrp->reverb.SetDecay(this->decay.getValue());
    };
    decay.setValue(lrp->reverb.GetDecay());

    addAndMakeVisible(damping);
    damping.setRange(0, 10);
    damping.onValueChange = [this, lrp]() {
        damping.nameLabel.setText("damping : " + juce::String(exp(this->damping.getValue())), juce::NotificationType::dontSendNotification);
        lrp->reverb.SetDamping(exp(this->damping.getValue()));
    };
    damping.setValue(log(lrp->reverb.GetDamping()));
    
    addAndMakeVisible(modulationDepth);
    modulationDepth.setRange(0, 1);
    modulationDepth.onValueChange = [this, lrp]() {
        modulationDepth.nameLabel.setText("mod depth : " + juce::String(this->modulationDepth.getValue()), juce::NotificationType::dontSendNotification);
        lrp->reverb.SetModulationDepth(this->modulationDepth.getValue());
    };
    modulationDepth.setValue(lrp->reverb.GetModulationDepth());
    
    dryWet.setRange(0, 1, 0);
    dryWet.onValueChange = [this, lrp]() {
        dryWet.nameLabel.setText("wet : " + juce::String(this->dryWet.getValue()), juce::NotificationType::dontSendNotification);
        lrp->reverb.SetDryWetRatio(this->dryWet.getValue());
    };
    dryWet.setValue(0.5);

    addAndMakeVisible(impulseButton);
    addAndMakeVisible(dryWet);
    impulseButton.onClick = [lrp]() {lrp->addInpulse();};

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
    flexbox.items = {juce::FlexItem( channelSelector).withMinWidth(120.0f).withMinHeight(90.0f),
        juce::FlexItem(dryWet).withMinWidth(120.0f).withMinHeight(90.0f),
        juce::FlexItem(roomSize).withMinWidth(120.0f).withMinHeight(90.0f),
        juce::FlexItem(roomShape).withMinWidth(120.0f).withMinHeight(90.0f),
        juce::FlexItem(decay).withMinWidth(120.0f).withMinHeight(90.0f),
        juce::FlexItem(damping).withMinWidth(120.0f).withMinHeight(90.0f),
        juce::FlexItem(modulationDepth).withMinWidth(120.0f).withMinHeight(90.0f)
    };
    flexbox.performLayout(getLocalBounds().getProportion(juce::Rectangle<float>(0, 0, 1, 0.8)).reduced(10));
    impulseButton.setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(0, 0.8, 0.5, 0.2)).reduced(10));

}