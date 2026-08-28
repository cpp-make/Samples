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
class DemoSynthesiserAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DemoSynthesiserAudioProcessorEditor (DemoSynthesiserAudioProcessor&);
    ~DemoSynthesiserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool keyPressed(const juce::KeyPress& key) override;

private:
    DemoSynthesiserAudioProcessor& audioProcessor;
    
    juce::Slider m_volumeSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> m_volumeSliderAttachment;

    juce::MidiKeyboardComponent m_midiKeyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoSynthesiserAudioProcessorEditor)
};
