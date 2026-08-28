/*
  ==============================================================================

    PostEffectAudioSource.cpp
    Created: 1 Mar 2025 8:15:11pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#include "PostEffectAudioSource.h"

// 6バンドイコライザーの各バンドの中心周波数を定義（低域から高域の順）
namespace
{
    constexpr std::array<double, 6> kFilterFrequencies
    {
        250.0,   // バンド1 - 低域
        500.0,   // バンド2 - 中低域
        1000.0,  // バンド3 - 中域
        2000.0,  // バンド4 - 中高域
        4000.0,  // バンド5 - 高域
        8000.0,  // バンド6 - 最高域
    };
}

//==============================================================================
// コンストラクタ - オーディオ処理チェーンを構築
PostEffectAudioSource::PostEffectAudioSource(juce::AudioSource* inputSource,
    bool deleteInputWhenDeleted)
    : inputAudioSource(inputSource, deleteInputWhenDeleted)
    , lastOutputGainDecibel(outputGainDecibel.load())
{
    // バンド数と中心周波数の数が一致していることをコンパイル時に確認する
    static_assert(kFilterFrequencies.size() == numFilters,
        "kFilterFrequencies must have one entry per band");

    // 6バンドイコライザーを構築する（6つの IIRフィルタを直列接続する）
    // 入力ソースは inputAudioSource、フィルタは iirFilterList が所有するため、
    // フィルタには所有権を渡さない（第二引数は常に false）
    juce::IIRFilterAudioSource* lastAudioSource = nullptr;
    for (int filter_index = 0; filter_index < numFilters; filter_index++)
    {
        if (filter_index == 0)
        {
            // 最初のフィルタの入力はこのクラスの外部にある入力ソースに接続する
            lastAudioSource = iirFilterList.add(
                new juce::IIRFilterAudioSource(inputAudioSource.get(), false));
        }
        else
        {
            // 後段のフィルタの入力は前段のフィルタの出力に接続する
            lastAudioSource = iirFilterList.add(
                new juce::IIRFilterAudioSource(lastAudioSource, false));
        }
    }

    // サンプリングレートの初期値を 48kHz に設定
    currentSampleRate = 48000;
}

// デストラクタ
PostEffectAudioSource::~PostEffectAudioSource()
{
    // juce::OwnedArray がフィルタを、juce::OptionalScopedPointer が
    // （所有している場合のみ）入力ソースを破棄するため、明示的な処理は不要
}

//==============================================================================
// オーディオ処理の準備を行う関数
void PostEffectAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // 最後のフィルタを介して全フィルタチェーンの準備を行う
    iirFilterList.getLast()->prepareToPlay(samplesPerBlockExpected, sampleRate);

    // フィルタ係数の計算に使うため、先にサンプリングレートを更新する
    currentSampleRate = sampleRate;

    // 各バンドのゲイン設定を保ったまま、新しいサンプリングレートで係数を作り直す
    for (uint32_t filter_index = 0; filter_index < numFilters; filter_index++)
    {
        setFilterGainDecibel(filter_index, filterGainDecibelList[filter_index].load());
    }
}

// オーディオリソースを解放する関数
void PostEffectAudioSource::releaseResources()
{
    // 最後のフィルタを介して全フィルタチェーンのリソースを解放
    iirFilterList.getLast()->releaseResources();
}

// オーディオデータを生成し、指定されたバッファに書き込む関数
void PostEffectAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // 最後のフィルタを介して全フィルタチェーンの処理を実行する
    iirFilterList.getLast()->getNextAudioBlock(bufferToFill);

    // 出力ゲインを適用する（クリックノイズを防ぐためにゲインランプを使用）
    const float last_linear_gain = juce::Decibels::decibelsToGain(lastOutputGainDecibel);
    const float linear_gain = juce::Decibels::decibelsToGain(outputGainDecibel.load());

    // 前回のゲイン値から現在のゲイン値へスムーズに遷移する（ゲインランプ）
    bufferToFill.buffer->applyGainRamp(0, bufferToFill.numSamples,
        last_linear_gain, linear_gain);

    // 次回のブロック処理のためにゲイン値を更新する
    lastOutputGainDecibel = outputGainDecibel;
}

//==============================================================================
// フィルタ数を取得する
uint32_t PostEffectAudioSource::getNumFilters()
{
    return numFilters;
}

// 指定されたフィルタのゲインをデシベル単位で設定する
void PostEffectAudioSource::setFilterGainDecibel(uint32_t index, double gainDecibel)
{
    // インデックスが有効範囲内であることを確認する
    jassert(index < numFilters);

    // 設定値を保持する（サンプリングレートが変わったときに係数を作り直すために使う）
    filterGainDecibelList[index] = gainDecibel;

    // フィルタのパラメータ（サンプリングレート、中心周波数、ゲイン係数）を設定する
    // Q値は Parameters のデフォルト値（1.0）をそのまま使う
    Parameters parameters;
    parameters.sampleRate = currentSampleRate;
    parameters.gainFactor = juce::Decibels::decibelsToGain(gainDecibel);
    parameters.frequency = getFilterFrequency(index);

    // 設定されたパラメータをフィルタオブジェクトに適用する
    setFilterParameters(index, parameters);
}

// 指定されたフィルタの周波数を取得
double PostEffectAudioSource::getFilterFrequency(uint32_t index) const
{
    // 無効なインデックスの場合は0を返す
    if (kFilterFrequencies.size() <= index)
    {
        return 0.0;
    }

    // インデックスに対応する中心周波数を返す
    return kFilterFrequencies[index];
}

//==============================================================================
// 出力ゲインをデシベル単位で設定
void PostEffectAudioSource::setOutputGainDecibel(double gainDecibel) noexcept
{
    outputGainDecibel = gainDecibel;
}

// 現在の出力ゲイン値をデシベル単位で取得
double PostEffectAudioSource::getOutputGainDecibel() const noexcept
{
    return outputGainDecibel;
}

//==============================================================================
// フィルタのパラメータをフィルタオブジェクトに適用する内部関数
void PostEffectAudioSource::setFilterParameters(uint32_t index, Parameters parameters)
{
    // インデックスが有効範囲内であることを確認する
    jassert(index < numFilters);

    // 指定されたインデックスのフィルタを取得する
    if (juce::IIRFilterAudioSource* audioSource = iirFilterList.getUnchecked(index))
    {
        // ピークフィルタ（特定の周波数帯域のみを増減させるフィルタ）の係数を生成
        const juce::IIRCoefficients coefficients =
            juce::IIRCoefficients::makePeakFilter(
                parameters.sampleRate,        // サンプリングレート
                parameters.frequency,         // 中心周波数
                parameters.Q,                 // Q値（フィルタの鋭さ）
                (float)parameters.gainFactor  // ゲイン係数
            );

        // 生成した係数をフィルタに適用する
        audioSource->setCoefficients(coefficients);
    }
}
