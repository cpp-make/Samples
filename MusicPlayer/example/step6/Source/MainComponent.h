#pragma once

#include <JuceHeader.h>
#include "WaveformView.h"
#include "PostEffectAudioSource.h"
#include "PostEffectController.h"
#include "TransportController.h"

//==============================================================================
/*
    このコンポーネントはウインドウ内に配置され、
    GUIコントロールやコンテンツを実装する場所です。
*/
// juce::AudioAppComponent を継承：オーディオ機能付きコンポーネントの基底クラス
// juce::FileDragAndDropTarget を継承：ファイルドラッグ＆ドロップ機能の基底クラス
// juce::ChangeListener を継承：juce::ChangeBroadcaster からの変更通知を受け取る基底クラス
// juce::ValueTree::Listener を継承：juce::ValueTree の変更を監視し通知を受け取る基底クラス
class MainComponent : public juce::AudioAppComponent
                    , public juce::FileDragAndDropTarget
                    , private juce::ChangeListener
                    , private juce::ValueTree::Listener
{
public:
    //==============================================================================
    // コンストラクタ：コンポーネント生成時の初期化を行う処理
    MainComponent();
    // デストラクタ：コンポーネント削除時の解放処理を行う
    ~MainComponent() override;

    //==============================================================================
    // juce::AudioAppComponent から継承した関数のうち、オーディオ処理に関連するもの
    // オーディオデバイスとの接続が開いた時に呼ばれる、再生準備操作を実行するコールバック関数
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    // 再生中に定期的に呼ばれ、バッファで指定された区間の
    // 波形生成処理を実行するコールバック関数
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    // オーディオデバイスとの接続が閉じた時に呼ばれ
    // リソース解放操作を実行するコールバック関数
    void releaseResources() override;

    //==============================================================================
    // juce::AudioAppComponent から継承した関数のうち、GUI実装に関連するもの
    // コンポーネントの描画を行う関数
    void paint (juce::Graphics& g) override;

    // コンポーネントのサイズが変更されたときに呼ばれる関数
    void resized() override;

    //==============================================================================
    // 音声ファイルを読み込む関数
    // 引数: fileToLoad - 読み込む音声ファイルへの参照
    void loadAudioFile(const juce::File& fileToLoad);

    // オーディオデバイス設定ダイアログを開く関数
    void showAudioDeviceSettingsDialog();

    // ファイル選択ダイアログを開く関数
    void openAudioFileChooser();

    // メニューバーモデルを取得する関数
    juce::MenuBarModel* getMenuBarModel() { return menuBarModel.get(); }

private:
    // juce::FileDragAndDropTarget から継承した関数
    // ドラッグされたファイルを受け入れるかどうかを判定する関数
    // 引数: files - ドラッグされたファイルの配列
    // 戻り値: 受け入れる場合は true、そうでない場合は false
    bool isInterestedInFileDrag(const juce::StringArray& files) override;

    // ファイルがドロップされたときに呼ばれる関数
    // 引数: files - ドロップされたファイルの配列
    // 引数: x, y - ドロップされた位置の座標
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    // juce::ChangeListener インターフェースの実装

    // トランスポートソースの状態変更を監視するコールバック関数
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // juce::ValueTree::Listener インターフェースの実装

    // juce::ValueTree のプロパティが変更された際に呼ばれるコールバック関数
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
        const juce::Identifier& propertyId) override;

    // オーディオ処理関連のメンバ変数

    // WAV、AIFF などの各種音声ファイル形式の読み込み処理の実装を提供するオブジェクト
    juce::AudioFormatManager audioFormatManager;

    // 音声ファイルの非同期読み込みを行うスレッドオブジェクト
    std::unique_ptr<juce::TimeSliceThread> audioBufferingThread;

    // 対応するフォーマットから音声データを読み込む
    // オーディオストリームを提供するオーディオソース
    std::unique_ptr<juce::AudioFormatReaderSource> audioFormatSource;

    // オーディオの再生、停止、シーク等の制御が可能なオーディオソースオブジェクト
    std::unique_ptr<juce::AudioTransportSource> transportSource;

    // 現在再生しているファイル名を表示するためのファイルオブジェクト
    juce::File currentPlayingAudioFile;
    
    // ユーザーガイダンステキストを表示する領域
    juce::Rectangle<int> rectTextMessage;

    // 生成した縮小データを保持し、同じファイルの再解析を省くキャッシュ
    // 引数は保持しておく音声ファイルの件数
    juce::AudioThumbnailCache audioThumbnailCache{ 5 };

    // 音声波形サムネイルを保持するオブジェクト
    std::unique_ptr<juce::AudioThumbnail> audioThumbnail;

    // 音声波形を表示および操作するための GUIコンポーネントクラス
    std::unique_ptr<WaveformView> waveformView;

    // オーディオ信号にエフェクトを適用する処理クラス
    std::unique_ptr<PostEffectAudioSource> postEffectAudioSource;

    // エフェクトのパラメータを調整する GUIコンポーネント
    std::unique_ptr<PostEffectController> postEffectController;

    // トランスポート操作コンポーネント - 再生/停止/ループの GUI を提供
    std::unique_ptr<TransportController> transportController;

    // アプリケーション全体の状態を管理するツリー - コンポーネント間の状態共有に使用
    juce::ValueTree applicationState{ "ApplicationState", {} };

    // ウインドウ直下に表示するメニューバーモデルのオブジェクト
    std::unique_ptr<juce::MenuBarModel> menuBarModel;

    // ファイル選択ダイアログのオブジェクト
    std::unique_ptr<juce::FileChooser> audioFileChooser;

    // コピー禁止とメモリリーク検出機能を有効化するマクロ
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
