/*
  ==============================================================================

    PostEffectAudioSource.h
    Created: 1 Mar 2025 8:15:11pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#pragma once

// JUCEライブラリの統合ヘッダーをインクルードする
#include <JuceHeader.h>

// PostEffectAudioSource
// オーディオ信号に対してエフェクト処理（イコライザーとゲイン調整）を行うクラス
// juce::AudioSource を継承：JUCE のオーディオ処理チェーンの
// 機構に組み込むことを可能にする基底クラス
class PostEffectAudioSource final
    : public juce::AudioSource
{
public:
    //==============================================================================
    // コンストラクタ
    // inputSource: エフェクト処理の入力となるオーディオソース
    // deleteInputWhenDeleted: このクラスが破棄される際に入力ソースも破棄するかどうか
    PostEffectAudioSource(juce::AudioSource* inputSource, bool deleteInputWhenDeleted);

    // デストラクタ
    ~PostEffectAudioSource() override;

    //==============================================================================
    // IIRフィルタ用のパラメータを保持する構造体
    struct Parameters
    {
        double sampleRate = 48000;  // サンプリングレート（デフォルト48kHz）
        double frequency = 12000;   // 中心周波数（デフォルト12kHz）
        double Q = 1.0f;            // フィルタの Q値（鋭さ/帯域幅）
        double gainFactor = 1.0f;   // ゲイン係数（リニアスケール）

        JUCE_LEAK_DETECTOR(Parameters)
    };

    // juce::AudioSource インターフェースの実装

    // オーディオ処理の準備を行う関数
    // samplesPerBlockExpected: 一度のオーディオコールバックで処理する予定のサンプル数
    // sampleRate: サンプリングレート（1秒あたりのサンプル数）
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // オーディオリソースを解放する関数
    void releaseResources() override;

    // オーディオデータを生成し、指定されたバッファに書き込む関数
    // bufferToFill: 書き込み先バッファへのポインタと、書き込むべき範囲
    //               （開始位置とサンプル数）をまとめた構造体
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    //==============================================================================
    // フィルタ数を取得する
    uint32_t getNumFilters();
    
    // 指定されたインデックスのフィルタのゲインをデシベル単位で設定する
    // index: 設定するフィルタのインデックス（0~5）
    // gainDecibel: 設定するゲイン値（デシベル単位）
    void setFilterGainDecibel(uint32_t index, double gainDecibel);

    // 指定されたインデックスのフィルタの周波数を取得する
    // index: 取得するフィルタのインデックス（0~5）
    // 戻り値: フィルタの中心周波数（Hz）
    double getFilterFrequency(uint32_t index) const;

    //==============================================================================
    // 出力ゲインをデシベル単位で設定する
    // gainDecibel: 設定するゲイン値（デシベル単位）
    void setOutputGainDecibel(double gainDecibel) noexcept;

    // 現在の出力ゲイン値をデシベル単位で取得する
    // 戻り値: 現在の出力ゲイン値（デシベル単位）
    double getOutputGainDecibel() const noexcept;

private:
    //==============================================================================
    // フィルタのパラメータを設定する内部関数
    // index: 設定するフィルタのインデックス
    // parameters: 設定するパラメータ
    void setFilterParameters(uint32_t index, Parameters parameters);

    //==============================================================================
    // エフェクト処理の入力となるオーディオソース
    // コンストラクタに deleteInputWhenDeleted = true を渡したときだけ、このクラスが破棄する
    juce::OptionalScopedPointer<juce::AudioSource> inputAudioSource;

    // フィルタオブジェクトのリスト（6バンドのイコライザーに使用）
    // フィルタは inputAudioSource を参照するだけなので破棄順に依存はないが、
    // 信号の流れ（入力ソースからフィルタへ）と同じ順序になるよう、あとに宣言する
    juce::OwnedArray<juce::IIRFilterAudioSource> iirFilterList;

    // 内部のフィルタオブジェクトの数を決める定数（6バンドイコライザー）
    static constexpr uint32_t numFilters{ 6 };

    // 各バンドのゲイン（デシベル単位）
    // サンプリングレートが変わったときに係数を作り直せるよう、設定値を保持する
    // GUIスレッドが書き込み、オーディオスレッドが読み出すため std::atomic にする
    std::array<std::atomic<double>, numFilters> filterGainDecibelList{};

    // 現在のサンプリングレート
    // オーディオスレッドが書き込み、GUIスレッドが読み出すため std::atomic にする
    std::atomic<double> currentSampleRate{ 0.0 };

    // 出力ゲイン（デシベル単位）- デフォルトは0.0dB
    // GUIスレッドが書き込み、オーディオスレッドが読み出すため std::atomic にする
    std::atomic<double> outputGainDecibel{ 0.0 };

    // 前回の出力ゲイン値（クリックノイズ防止のためのスムージング用）
    // オーディオスレッドからしか触らないため、通常の double でよい
    double lastOutputGainDecibel;

    // コピー禁止とメモリリーク検出機能を有効化するマクロ
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PostEffectAudioSource)
};
