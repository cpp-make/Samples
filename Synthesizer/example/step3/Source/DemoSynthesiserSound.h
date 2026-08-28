/*
  ==============================================================================

    DemoSynthesiserSound.h
    Author:  migizo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SynthesiserSound
: public juce::SynthesiserSound
{
public:
    SynthesiserSound(std::atomic<float>& volumeParam)
    : volume(volumeParam)
    {
    }
    ~SynthesiserSound() {}
    
    /// @brief 引数に渡されたMIDIノートに対応させるかどうか。条件分岐をせずtrueを返すことで常に対応させている。
    bool appliesToNote (int midiNoteNumber) override    { return true; }
    
    /// @brief 引数に渡されたMIDIチャンネルに対応させるかどうか。条件分岐をせずtrueを返すことで常に対応させている。
    bool appliesToChannel (int midiChannel) override    { return true; }
    
    float getVolume() const { return juce::Decibels::decibelsToGain(volume.load()); }

private:
    std::atomic<float>& volume;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserSound)
};
