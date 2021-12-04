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
    : AudioProcessorEditor(&p), audioProcessor(p), channelSelector(audioProcessor, "channel : -1"), cutoff(audioProcessor, "cutoff"),
    impulseButton("impulse"), dryWet(audioProcessor,"")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible(channelSelector);
    cutoff.setRange(0,11,0);
    
    cutoff.onValueChange = [this, &p]() {
        cutoff.nameLabel.setText("cutoff : " + juce::String(exp(this->cutoff.getValue())), juce::NotificationType::dontSendNotification);
        p.reverb.delayFilters.setA(exp(this->cutoff.getValue()));
    };
    cutoff.setValue(0.5);

    channelSelector.setRange(-1,7,1);
    channelSelector.setValue(-1);
    channelSelector.onValueChange = [this,&p]() {
        channelSelector.nameLabel.setText("channel   : "+juce::String(this->channelSelector.getValue()), juce::NotificationType::dontSendNotification);
        p.reverb.ChangeChannel(this->channelSelector.getValue());
    };

    dryWet.setRange(0, 1, 0);
    dryWet.onValueChange = [this, &p]() {
        dryWet.nameLabel.setText("wet : " + juce::String(this->dryWet.getValue()), juce::NotificationType::dontSendNotification);
        p.reverb.SetDryWetRatio(this->dryWet.getValue());
    };
    dryWet.setValue(0.5);

    addAndMakeVisible(cutoff);
    addAndMakeVisible(impulseButton);
    addAndMakeVisible(dryWet);
    impulseButton.onClick = [&p]() {p.addInpulse();};
    chooseButton.onClick = [&p]() {p.chooseFile(); };

    addAndMakeVisible(chooseButton);
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
    flexbox.items = {juce::FlexItem( channelSelector).withMinWidth(90.0f).withMinHeight(90.0f),
        juce::FlexItem(cutoff).withMinWidth(90.0f).withMinHeight(90.0f),
        juce::FlexItem(dryWet).withMinWidth(90.0f).withMinHeight(90.0f)
    };
    flexbox.performLayout(getLocalBounds().getProportion(juce::Rectangle<float>(0, 0, 1, 0.8)).reduced(10));
    impulseButton.setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(0, 0.8, 0.5, 0.2)).reduced(10));

    chooseButton.setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(0.5, 0.8, 0.5, 0.2)).reduced(10));
}
