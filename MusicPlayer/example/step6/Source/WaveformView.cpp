/*
  ==============================================================================

    WaveformView.cpp
    Created: 28 Feb 2025 12:13:31am
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#include "WaveformView.h"

//==============================================================================
// WaveformView のコンストラクタ：音声サムネイルとトランスポートソースを初期化する
WaveformView::WaveformView(juce::AudioThumbnail& audioThumbnail,
    juce::AudioTransportSource& transportSource)
    : audioThumbnailRef(audioThumbnail)
    , transportSourceRef(transportSource)
{
    // 音声サムネイルの変更を監視するリスナーを追加登録する
    audioThumbnailRef.addChangeListener(this);

    // 現在の再生位置マーカーの色と透明度を設定
    currentPositionMarker.setFill(juce::Colours::yellow.withAlpha(0.85f));
    addAndMakeVisible(currentPositionMarker);

    // 現在の再生位置をテキストで表示するラベルの作成と初期設定
    labelPlayingPosition = std::make_unique<juce::Label>();
    labelPlayingPosition->setFont(juce::Font(14.0f));
    labelPlayingPosition->setJustificationType(juce::Justification::centred);
    labelPlayingPosition->setColour(juce::Label::ColourIds::backgroundColourId,
        juce::Colours::black.withAlpha(0.6f));
    labelPlayingPosition->setColour(juce::Label::ColourIds::textColourId,
        juce::Colours::white.withAlpha(0.8f));
    addAndMakeVisible(labelPlayingPosition.get());

    // 1秒間に30回タイマーコールバックを呼び出す
    startTimerHz(30);
}

// デストラクタ：コンポーネント削除時の解放処理を行う
WaveformView::~WaveformView()
{
    // 音声サムネイルに登録された変更を監視するリスナーを削除する
    audioThumbnailRef.removeChangeListener(this);
}

//==============================================================================
// コンポーネントの描画を行う関数
void WaveformView::paint(juce::Graphics& g)
{
    // 背景を濃いグレーで塗りつぶす
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightblue);

    // 音声サムネイルの長さが0より大きい場合、波形を描画する
    if (audioThumbnailRef.getTotalLength() > 0.0)
    {
        // 再生位置表示ラベルを除いた矩形領域を取得する
        const auto thumbArea = getLocalBounds().withTrimmedBottom(24);

        // 矩形領域に音声波形サムネイルを描画する
        audioThumbnailRef.drawChannels(g, thumbArea.reduced(2), 0.0f,
            audioThumbnailRef.getTotalLength(), 1.0f);
    }
}

// コンポーネントのサイズが変更されたときに呼ばれる関数
void WaveformView::resized()
{
    // このコンポーネント内のローカル座標系の矩形領域を取得する
    auto rectLocal = getLocalBounds();

    // 再生位置を表示するラベルをコンポーネントの下部に配置する
    labelPlayingPosition->setBounds(rectLocal.removeFromBottom(24));

    // このコンポーネントの再描画を要求する
    repaint();
}

//==============================================================================
// マウスボタン押下時のイベント処理
void WaveformView::mouseDown(const juce::MouseEvent& e)
{
    // 今回はマウスドラッグイベントと同じ処理を呼び出す
    mouseDrag(e);
}

// マウスドラッグ中のイベント処理
void WaveformView::mouseDrag(const juce::MouseEvent& e)
{
    // マウスカーソルの位置から再生位置を計算し、トランスポートソースの位置を設定する
    const auto position = juce::jmax(0.0,
        xToTime((float)e.x, getWidth(), transportSourceRef.getLengthInSeconds()));
    transportSourceRef.setPosition(position);
}

// マウスボタンリリース時のイベント処理
void WaveformView::mouseUp(const juce::MouseEvent&)
{
    // トランスポートソースの再生を開始する
    transportSourceRef.start();
}

//==============================================================================
// 音声サムネイルの変更を監視するコールバック関数
void WaveformView::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // サムネイルが変更された場合、このコンポーネントの再描画を要求する
    if (source == &audioThumbnailRef)
    {
        repaint();
    }
}

//==============================================================================
// 定期的に呼び出されるタイマーコールバック関数
void WaveformView::timerCallback()
{
    // 音声の長さが0より大きく、再生中の場合
    if (transportSourceRef.getLengthInSeconds() > 0.0 && transportSourceRef.isPlaying())
    {
        // 現在の再生位置マーカーを表示
        currentPositionMarker.setVisible(true);

        // マーカーの太さを設定する
        const float thickness = 1.5f;

        // 現在の再生位置の X座標を計算する
        const float x = timeToX(transportSourceRef.getCurrentPosition(),
            transportSourceRef.getLengthInSeconds(), getWidth()) - thickness * 0.5f;
        
        // マーカーの高さをコンポーネントの高さから現在位置表示ラベルの高さを減じた値に設定する
        const float height = (float)this->getHeight() - labelPlayingPosition->getHeight();

        // マーカーの矩形を作成する
        const juce::Rectangle<float> current_marker_rect =
            juce::Rectangle<float>{ x, 0, thickness, height };

        // マーカーの位置を更新する
        currentPositionMarker.setRectangle(current_marker_rect);

        // 現在の再生位置を示すテキストを更新する
        const juce::String text_playing_position =
            formatTime(transportSourceRef.getCurrentPosition()) + " / "
            + formatTime(transportSourceRef.getLengthInSeconds());
        labelPlayingPosition->setText(text_playing_position, juce::dontSendNotification);
    }
    else
    {
        // 再生していない場合、マーカーを非表示にする
        currentPositionMarker.setVisible(false);

        // 現在の再生位置を示すテキストを更新する
        const juce::String text_playing_position =
            formatTime(0.0) + " / " + formatTime(0.0);
        labelPlayingPosition->setText(text_playing_position, juce::dontSendNotification);
    }

    // このコンポーネントの再描画を要求する
    repaint();
}

//==============================================================================
// 時間座標からピクセル座標への変換ユーティリティ関数
float WaveformView::timeToX(double targetTime, double totalTime, float width)
{
    return (float)width * (float)(targetTime) / (float)totalTime;
}

// ピクセル座標から時間座標への変換ユーティリティ関数
double WaveformView::xToTime(float targetX, float width, double totalTime)
{
    return (targetX / width) * (float)totalTime;
}

// 秒数から時間表記の文字列への変換ユーティリティ関数
juce::String WaveformView::formatTime(double timeInSeconds)
{
    auto timeSec = juce::RelativeTime::seconds(timeInSeconds);

    juce::String result;
    result = juce::String::formatted("%02d:%02d:%02d", 
        (int)timeSec.inHours(), 
        (int)timeSec.inMinutes() % 60, 
        (int)timeSec.inSeconds() % 60);

    return result;
}
