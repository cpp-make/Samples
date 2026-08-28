/*
  ==============================================================================

    DemoSynthesiserVoice.cpp
    Author:  migizo

  ==============================================================================
*/

#include "DemoSynthesiserVoice.h"
bool SynthesiserVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return sound != nullptr;
}

void SynthesiserVoice::setCurrentPlaybackSampleRate(double sampleRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(sampleRate);
    
    // 0.01秒で各値を変化させるようにする
    const int numSmoothSamples = sampleRate * 0.01;
    m_fade.reset(numSmoothSamples);
}

void SynthesiserVoice::startNote (int midiNoteNumber, float velocity,
                                  juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    // 各値のリセット
    m_currentAngle = 0;
    m_velocity = velocity * 0.2;
    m_fade.setTargetValue(1.0);
    
    // MIDIノートナンバから音声波形計算用の1サンプルごとに加算するための角度値を求める
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();
    m_angleDelta = cyclesPerSample * Math::twoPi;
}

void SynthesiserVoice::stopNote (float velocity, bool allowTailOff)
{
        // ボイススチールされた場合の処理。ボイス状態をリセットする。
    if (allowTailOff == false)
    {
        clearCurrentNote();
        m_angleDelta = 0.0;
    }
    // ボイススチールされていない場合のノートオフ処理
    else
    {
        m_fade.setTargetValue(0.0);
    }
}

void SynthesiserVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    // 1サンプルごとに加算するための角度値が0なら処理をスキップする
    if (m_angleDelta == 0.0f) return;
    
    // 何らかのノートが押されているか、ノートが離されたがリリース状態である場合に処理を行う
    if (isKeyDown() || isPlayingButReleased())
    {
        // 音量を50%の値になるように指定。
        float volume = 0.5f;
        
        for (int sample = 0; sample < numSamples; sample++)
        {
            // スムーズに変化させる値の更新
            double fade = m_fade.getNextValue();

            // 出力バッファに書き込み
            for (auto ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            {
                double sampleData = processOscillator(m_currentAngle) * m_velocity * fade * volume;
                outputBuffer.addSample (ch, startSample + sample, sampleData);
            }
            
            // tailOff後のoff処理
            if (juce::approximatelyEqual(fade, 0.0))
            {
                clearCurrentNote();
                m_angleDelta = 0.0;
                break;
            }
            
            // 角度の更新
            m_currentAngle += m_angleDelta;
        }
    }
}

double SynthesiserVoice::processOscillator(double angle)
{
    return std::sin(angle); // サイン波
}
