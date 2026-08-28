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
    SynthesiserSound(std::atomic<float>& volumeParam,
                     std::atomic<float>& typeParam)
    : volume(volumeParam)
    , type(typeParam)
    {
    }
    
    ~SynthesiserSound() {}
    
    /// @brief 引数に渡されたMIDIノートに対応させるかどうか。条件分岐をせずtrueを返すことで常に対応させている。
    bool appliesToNote (int midiNoteNumber) override    { return true; }
    
    /// @brief 引数に渡されたMIDIチャンネルに対応させるかどうか。条件分岐をせずtrueを返すことで常に対応させている。
    bool appliesToChannel (int midiChannel) override    { return true; }
    
    /// @brief 波形種別
    enum WaveType
    {
        Sine = 0,
        Saw,
        Square,
        Triangle,
        Random
    };
    
    /// @brief 各種波形名を表す文字列の配列を返す。
    static juce::StringArray GetWaveTypeList()
    {
        juce::StringArray waveTypeList;
        waveTypeList.add("sine");
        waveTypeList.add("saw");
        waveTypeList.add("square");
        waveTypeList.add("triangle");
        waveTypeList.add("random");
        return waveTypeList;
    }
    
    float getVolume() const { return juce::Decibels::decibelsToGain(volume.load()); }
    WaveType getType() const { return (WaveType)type.load(); }

private:
    std::atomic<float>& volume;
    std::atomic<float>& type;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserSound)
};
