/*
  ==============================================================================

    TransportController.h
    Created: 2 Mar 2025 1:20:10pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#pragma once

// JUCEライブラリの統合ヘッダーをインクルードする
#include <JuceHeader.h>

// TransportController
// 音声の再生・停止・ループ機能を制御する GUIコンポーネント
// juce::Component を継承：GUIコンポーネントの基本的な機能
// （描画、サイズ変更、マウスイベントなど）を提供する基底クラス
// juce::ValueTree::Listener を継承：juce::ValueTree の変更を監視し通知を受け取る基底クラス
class TransportController final
    : public juce::Component
    , private juce::ValueTree::Listener
{
public:
    // コンストラクタ - アプリケーションの状態管理ツリーを参照として受け取る
    // 引数では juce::ValueTree の参照を渡しているが、内部で値コピーをして参照カウントを追加する
    explicit TransportController(juce::ValueTree& appState);

    // デストラクタ - 状態管理ツリーのリスナー登録の解除などを行う
    ~TransportController() override;

private:
    // juce::Component インターフェースの実装

    // コンポーネントの描画を行う関数
    void paint(juce::Graphics& g) override;

    // コンポーネントのサイズが変更されたときに呼ばれる関数
    void resized() override;

    // juce::ValueTree::Listener インターフェースの実装

    // juce::ValueTree のプロパティが変更された際に呼ばれるコールバック関数
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
        const juce::Identifier& propertyId);

    // このクラス独自のメンバ関数

    // GUIコンポーネントの表示状態を最新の状態変数の値に基づいて更新する
    void updateView();

    // グループコンポーネント - 再生制御GUI をグループ化する外枠コンテナ
    std::unique_ptr<juce::GroupComponent> group;

    // 再生/停止を切り替えるボタンコンポーネント
    std::unique_ptr<juce::TextButton> playButton;

    // ループ再生を切り替えるボタンコンポーネント
    std::unique_ptr<juce::TextButton> loopButton;

    // アプリケーション状態を管理する juce::ValueTree オブジェクト
    // オブジェクトの内部データの実体は参照カウントで管理されており
    // 内部データの実体は MainComponent 側で持つ
    // juce::ValueTree オブジェクトであることを期待している
    juce::ValueTree applicationState;

    // 再生可能状態（音声ファイルが読み込まれているか）を管理する状態値
    juce::CachedValue<bool> valueCanPlay;

    // 現在の再生状態（再生中かどうか）を管理する状態値
    juce::CachedValue<bool> valueIsPlaying;

    // ループ再生モードの有効/無効を管理する状態値
    juce::CachedValue<bool> valueIsLooping;

    // コピー禁止とメモリリーク検出機能を有効化するマクロ
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportController)
};
