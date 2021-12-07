/*
  ==============================================================================

    LateReverbEditor.cpp
    Created: 7 Dec 2021 2:12:38pm
    Author:  eri24816

  ==============================================================================
*/

#include "LateReverbEditor.h"
//#include "PluginProcessor.h"


RotarySlider::RotarySlider(char* varName) :nameLabel("nameLabel", varName) {
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    setTextBoxStyle(juce::Slider::TextBoxBelow,
        true,
        getTextBoxWidth(),
        getTextBoxHeight() * 2);

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
LateReverbEditor::LateReverbEditor(LateReverbProcessor& p)
    : audioProcessor(p), channelSelector("channel : -1"), roomSize(""), roomShape( ""), decay( ""), damping( ""), modulationDepth(""),
    impulseButton("impulse"), dryWet("")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(700, 300);
    addAndMakeVisible(channelSelector);


    addAndMakeVisible(roomSize);
    roomSize.setRange(1, 5000);
    roomSize.onValueChange = [this, &p]() {
        roomSize.nameLabel.setText("room size : " + juce::String(this->roomSize.getValue()), juce::NotificationType::dontSendNotification);
        p.reverb.SetRoomSize(this->roomSize.getValue());
    };
    roomSize.setValue(p.reverb.GetRoomSize());

    addAndMakeVisible(roomShape);
    roomShape.setRange(0, 1);
    roomShape.onValueChange = [this, &p]() {
        roomShape.nameLabel.setText("room shape : " + juce::String(this->roomShape.getValue()), juce::NotificationType::dontSendNotification);
        p.reverb.SetRoomShape(this->roomShape.getValue());
    };
    roomShape.setValue(p.reverb.GetRoomShape());

    addAndMakeVisible(decay);
    decay.setRange(0, 1);
    decay.onValueChange = [this, &p]() {
        decay.nameLabel.setText("decay : " + juce::String(this->decay.getValue()), juce::NotificationType::dontSendNotification);
        p.reverb.SetDecay(this->decay.getValue());
    };
    decay.setValue(p.reverb.GetDecay());

    addAndMakeVisible(damping);
    damping.setRange(0, 10);
    damping.onValueChange = [this, &p]() {
        damping.nameLabel.setText("damping : " + juce::String(exp(this->damping.getValue())), juce::NotificationType::dontSendNotification);
        p.reverb.SetDamping(exp(this->damping.getValue()));
    };
    damping.setValue(log(p.reverb.GetDamping()));

    addAndMakeVisible(modulationDepth);
    modulationDepth.setRange(0, 1);
    modulationDepth.onValueChange = [this, &p]() {
        modulationDepth.nameLabel.setText("mod depth : " + juce::String(this->modulationDepth.getValue()), juce::NotificationType::dontSendNotification);
        p.reverb.SetModulationDepth(this->modulationDepth.getValue());
    };
    modulationDepth.setValue(p.reverb.GetModulationDepth());

    dryWet.setRange(0, 1, 0);
    dryWet.onValueChange = [this, &p]() {
        dryWet.nameLabel.setText("wet : " + juce::String(this->dryWet.getValue()), juce::NotificationType::dontSendNotification);
        p.reverb.SetDryWetRatio(this->dryWet.getValue());
    };
    dryWet.setValue(0.5);

    addAndMakeVisible(impulseButton);
    addAndMakeVisible(dryWet);
    impulseButton.onClick = [&p]() {p.addInpulse(); };

}

LateReverbEditor::~LateReverbEditor()
{
}

//==============================================================================
void LateReverbEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void LateReverbEditor::resized()
{
    juce::FlexBox flexbox;
    flexbox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexbox.justifyContent = juce::FlexBox::JustifyContent::center;
    flexbox.alignContent = juce::FlexBox::AlignContent::center;
    flexbox.items = { juce::FlexItem(channelSelector).withMinWidth(120.0f).withMinHeight(90.0f),
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