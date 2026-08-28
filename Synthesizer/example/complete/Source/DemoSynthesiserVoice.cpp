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
    m_volume.reset(numSmoothSamples);
    m_phase.reset(numSmoothSamples);
}

void SynthesiserVoice::startNote (int midiNoteNumber, float velocity,
                                  juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    // 各値のリセット
    m_currentAngle = 0;
    m_velocity = velocity * 0.2;
    m_fade.setTargetValue(1.0);
    m_lastValue.fill(0.0);

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
        auto sound = dynamic_cast<SynthesiserSound*>(getCurrentlyPlayingSound().get());
        m_volume.setTargetValue (sound->getVolume());
        m_phase.setTargetValue (sound->getPhase());
        WaveType waveType = sound->getType();

        for (int sample = 0; sample < numSamples; sample++)
        {
            // スムーズに変化させる値の更新
            double fade = m_fade.getNextValue();
            double volume = m_volume.getNextValue();
            double phase = m_phase.getNextValue() * Math::pi;

            // 出力バッファに書き込み
            for (auto ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            {
                double sampleData = processSample(m_currentAngle + (double)ch * phase, waveType, ch) * m_velocity * fade * volume;
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

double SynthesiserVoice::processSample(double angle, WaveType waveType, int ch)
{
    double dt = m_angleDelta / Math::twoPi;
    double t = std::fmod(angle / Math::twoPi, 1.0); // 0 <= t < 1
    double value = 0.0;
    
    // ノコギリ波
    if (waveType == WaveType::Saw)
    {
        value = processOscillator(angle, waveType);
        value -= PolyBlep(t, dt);
    }
    // 矩形波および三角波
    else if (waveType == WaveType::Square || waveType == WaveType::Triangle)
    {
        value = processOscillator(angle, WaveType::Square);
        value += PolyBlep(t, dt);
        value -= PolyBlep(std::fmod(t + 0.5, 1.0), dt);
        // 三角波
        if (waveType == WaveType::Triangle)
        {
            value = m_angleDelta * value + (1.0 - m_angleDelta) * m_lastValue[ch];
            m_lastValue[ch] = value;
        }
    }
    // サイン波およびランダムノイズ
    else
    {
        value = processOscillator(angle, waveType);
    }
    return value;
}

double SynthesiserVoice::processOscillator(double angle, WaveType waveType)
{
    double value = 0.0;
    switch(waveType)
    {
        // サイン波
        case WaveType::Sine:
            value = std::sin(angle);
            break;
        // ノコギリ波
        case WaveType::Saw:
        {
            double x = angle / Math::twoPi;
            x -= std::floor(x);
            value = x * 2.0 - 1.0;
            break;
        }
        // 矩形波
        case WaveType::Square:
        {
            double x = angle / Math::twoPi;
            x -= std::floor(x);
            value = (x < 0.5) ? 1.0 : -1.0;
            break;
        }
        // 三角波
        case WaveType::Triangle:
        {
            double x = angle / Math::twoPi - 0.25;
            x -= std::floor(x);
            value = 4.0 * std::abs(x - 0.5) - 1.0;
            break;
        }
        // ランダムノイズ
        case WaveType::Random:
            value = juce::Random::getSystemRandom().nextFloat() * 2.0 - 1.0;
            break;
        default:
            break;
    }
    return value;
}

double SynthesiserVoice::PolyBlep(double t, double dt)
{
    // 引数tの値は 0 <= t < 1 の想定

    if (t < dt)
    {
        t /= dt;            // 0 <= t < 1
        return -t * t + 2. * t - 1.;
    }

    else if (1. - dt < t)
    {
        t = (t - 1.) / dt;  // -1 < t < 0
        return t * t + 2. * t + 1.;
    }

    return 0.;
}
