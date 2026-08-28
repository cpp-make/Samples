/*
  ==============================================================================

    PostEffectController.h
    Created: 1 Mar 2025 8:15:50pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#pragma once

// JUCEライブラリの統合ヘッダーをインクルードする
#include <JuceHeader.h>

// このコンポーネントが制御対象とする信号処理クラスの宣言をインクルードする
#include "PostEffectAudioSource.h"

// PostEffectController
// イコライザーと出力ゲインを制御するための GUIコンポーネント
// juce::Component を継承：GUIコンポーネントの基本的な機能
// （描画、サイズ変更、マウスイベントなど）を提供する基底クラス
class PostEffectController final
    : public juce::Component
{
public:
    //==============================================================================
    // コンストラクタ
    // audioSource: 制御対象の PostEffectAudioSource への参照
    explicit PostEffectController(PostEffectAudioSource& audioSource);

    // デストラクタ
    ~PostEffectController() override;

private:
    //==============================================================================
    // juce::Component インターフェースの実装
    
    // コンポーネントの描画を行う関数
    // g: 描画に使用するグラフィックスコンテキスト
    void paint(juce::Graphics& g) override;

    // コンポーネントのサイズが変更されたときに呼ばれる関数
    void resized() override;

    //==============================================================================
    // 制御対象のオーディオエフェクトへの参照
    PostEffectAudioSource& postEffectAudioSourceRef;

    // イコライザーコントロール用の GUIコンポーネント
    // イコライザーコントロールをグループ化するコンポーネント
    std::unique_ptr<juce::GroupComponent> groupFilterGain;
    // 各バンドの周波数表示用ラベルのリスト
    juce::OwnedArray<juce::Label> labelFrequencyList;
    // 各バンドのゲイン調整用スライダーのリスト
    juce::OwnedArray<juce::Slider> sliderFilterGainList;

    // 出力ゲインコントロール用の GUIコンポーネント
    // 出力ゲインコントロールをグループ化するコンポーネント
    std::unique_ptr<juce::GroupComponent> groupOutputGain;
    // 出力ゲイン調整用スライダー
    std::unique_ptr<juce::Slider> sliderOutputGain;

    // コピー禁止とメモリリーク検出機能を有効化するマクロ
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PostEffectController)
};
