/*
  ==============================================================================

    WaveformView.h
    Created: 28 Feb 2025 12:13:31am
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#pragma once

// JUCEライブラリの統合ヘッダーをインクルードする
#include <JuceHeader.h>

// 音声波形を表示および操作するための GUIコンポーネントクラス
// juce::Component を継承：GUIコンポーネントの基本的な機能
// （描画、サイズ変更、マウスイベントなど）を提供する基底クラス
// juce::ChangeListener を継承：juce::ChangeBroadcaster からの変更通知を受け取る基底クラス
// juce::Timer を継承：定期的なコールバックを実現するイベント機能を提供する基底クラス
class WaveformView final
    : public juce::Component
    , private juce::ChangeListener
    , private juce::Timer
{
public:
    // コンストラクタ：音声サムネイルとトランスポートソースを受け取り、波形表示を初期化する
    WaveformView(juce::AudioThumbnail& audioThumbnail,
        juce::AudioTransportSource& transportSource);

    // デストラクタ：コンポーネント削除時の解放処理を行う
    ~WaveformView() override;

private:
    // juce::Component インターフェースの実装

    // コンポーネントの描画を行う関数
    void paint(juce::Graphics& g) override;

    // コンポーネントのサイズが変更されたときに呼ばれる関数
    void resized() override;

    // マウスボタン押下時のイベント処理
    void mouseDown(const juce::MouseEvent& e) override;

    // マウスドラッグ中のイベント処理
    void mouseDrag(const juce::MouseEvent& e) override;

    // マウスボタンリリース時のイベント処理
    void mouseUp(const juce::MouseEvent&) override;

    // juce::ChangeListener インターフェースの実装

    // 音声サムネイルの変更を監視するコールバック関数
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // juce::Timer インターフェースの実装

    // 定期的に呼び出されるタイマーコールバック関数
    void timerCallback() override;

    // 時間座標からピクセル座標への変換ユーティリティ関数
    static float timeToX(double targetTime, double totalTime, float width);

    // ピクセル座標から時間座標への変換ユーティリティ関数
    static double xToTime(float targetX, float width, double totalTime);

    // 秒数から時間表記の文字列への変換ユーティリティ関数
    static juce::String formatTime(double timeInSeconds);

    // 現在の再生位置を示すマーカー
    juce::DrawableRectangle currentPositionMarker;

    // 現在の再生位置をテキストで表示するラベル
    std::unique_ptr<juce::Label> labelPlayingPosition;

    // 他のクラスが所有するオブジェクトの参照
    // 音声サムネイルへの参照
    juce::AudioThumbnail& audioThumbnailRef;

    // トランスポートソースへの参照
    juce::AudioTransportSource& transportSourceRef;

    // コピー禁止とメモリリーク検出機能を有効化するマクロ
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformView)
};
