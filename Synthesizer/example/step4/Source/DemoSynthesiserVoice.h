/*
  ==============================================================================

    DemoSynthesiserVoice.h
    Author:  migizo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DemoSynthesiserSound.h"

class SynthesiserVoice  : public juce::SynthesiserVoice
{
public:
    SynthesiserVoice() {}
    ~SynthesiserVoice() {}
    
    /// @brief 引数に渡されたSoundに対応させ鳴らす必要がある場合にtrueを返す関数
    /// @param sound 確認する対象のSound
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    
    /// @brief サンプリング周波数がセットされた時に呼ばれる関数
    /// @param sampleRate 新しいサンプリング周波数
    void setCurrentPlaybackSampleRate(double sampleRate) override;

    /// @brief ノートオンになった際に呼ばれる関数
    /// @param midiNoteNumber ノートオン時のMIDIノートナンバー
    /// @param velocity ノートオン時のベロシティ
    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    /// @brief ノートオフになった際に呼ばれる関数
    /// @param velocity ノートが離された速さ。値が大きいほどノートを素早く離したことを指す。
    /// @param allowTailOff ノートを離した場合の音の減衰処理の実装が行えるかどうか。falseの場合は減衰処理を行わずリセットする。
    void stopNote (float velocity, bool allowTailOff) override;

    /// @brief 引数に渡されたオーディオバッファに音声を書き込む処理
    /// @param outputBuffer 出力音声の格納先
    /// @param startSample オーディオバッファ開始位置
    /// @param numSamples オーディオバッファのサイズ
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    /// @brief ピッチホイールが変更された時に呼ばれる関数。今回は何もしない。
    void pitchWheelMoved (int newValue) override {}
    /// @brief MIDI CCが変更された時に呼ばれる関数。今回は何もしない。
    void controllerMoved (int controllerNumber, int newValue) override {}

private:
    using Math = juce::MathConstants<double>;
    using WaveType = SynthesiserSound::WaveType;

    // 1サンプル分の波形の計算
    double processSample(double angle, WaveType waveType);
    double processOscillator(double angle, WaveType waveType);
    static double PolyBlep(double t, double dt);
    
    /// @brief 音声波形計算用の現在の角度値
    double m_currentAngle = 0.0;
    /// @brief 音声波形計算用の1サンプルごとに加算するための角度値
    double m_angleDelta = 0.0;
    /// @brief ノートのベロシティ
    double m_velocity = 0.0;
    /// @brief 三角波で使用する過去の出力値
    double m_lastValue = 0.0;

    /// @brief ノートオン/オフ時に音をスムーズにする
    juce::SmoothedValue<double> m_fade {0.0};
    /// @brief volumeパラメータ変更時に音をスムーズにする
    juce::SmoothedValue<double> m_volume {0.0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserVoice)
};


