/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DemoSynthesiserAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    DemoSynthesiserAudioProcessorEditor (DemoSynthesiserAudioProcessor&);
    ~DemoSynthesiserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool keyPressed(const juce::KeyPress& key) override;

private:
    void timerCallback() override;

    DemoSynthesiserAudioProcessor& audioProcessor;
    AudioBufferFifo& m_stereoFifo;

    juce::Slider m_volumeSlider;
    juce::ComboBox m_typeComboBox;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<SliderAttachment> m_volumeSliderAttachment;
    std::unique_ptr<ComboBoxAttachment> m_typeComboBoxAttachment;
    
    juce::MidiKeyboardComponent m_midiKeyboardComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoSynthesiserAudioProcessorEditor)
};
