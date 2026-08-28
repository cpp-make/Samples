#include "MainComponent.h"

//==============================================================================
// MainComponent クラスのコンストラクタ定義
MainComponent::MainComponent()
{
    // ※ ここから: オーディオデバイスを初期化する前に実行する必要がある

    // JUCE が提供する基本的なオーディオ形式（WAV、AIFF等）の読み込みサポートを追加する
    audioFormatManager.registerBasicFormats();

    // 音声ファイルの非同期読み込み用スレッドの作成と開始を行う
    audioBufferingThread = std::make_unique<juce::TimeSliceThread>("AudioBufferingThread");
    audioBufferingThread->startThread();

    // オーディオ再生制御機能を有するトランスポートソースを作成する
    transportSource = std::make_unique<juce::AudioTransportSource>();

    // オーディオエフェクト処理を行うオーディオソースオブジェクトを追加する
    postEffectAudioSource =
        std::make_unique<PostEffectAudioSource>(transportSource.get(), false);

    // ※ ここまで: オーディオデバイスを初期化する前に実行する必要がある

    // ※ ここから: ウインドウサイズを設定する前に実行する必要がある

    // 音声波形サムネイルを保持するオブジェクトを作成する
    audioThumbnail = std::make_unique<juce::AudioThumbnail>(
        512, audioFormatManager, audioThumbnailCache);

    // 波形表示コンポーネントを作成する
    waveformView = std::make_unique<WaveformView>(
        *audioThumbnail.get(), *transportSource.get());
    addAndMakeVisible(waveformView.get());

    postEffectController = std::make_unique<PostEffectController>(*postEffectAudioSource.get());
    addAndMakeVisible(postEffectController.get());

    // ※ ここまで: ウインドウサイズを設定する前に実行する必要がある

    // ウインドウサイズを幅800ピクセル、高さ600ピクセルに設定する
    setSize(800, 600);

    // オーディオデバイスを指定したチャンネル数で初期化する
    // 再生機能だけを実装するため
    // 入力チャンネル数0（入力なし）、出力チャンネル数2（ステレオ出力）に設定
    setAudioChannels(0, 2);
}

// MainComponent クラスのデストラクタ定義
MainComponent::~MainComponent()
{
    // オーディオデバイスを停止し、オーディオソースをクリアする
    shutdownAudio();
}

//==============================================================================
// オーディオ処理の準備を行う関数
// samplesPerBlockExpected: 一度のオーディオコールバックで処理する予定のサンプル数
// sampleRate: サンプリングレート（1秒あたりのサンプル数）
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // 変更前: transportSource->prepareToPlay(samplesPerBlockExpected, sampleRate);

    // オーディオエフェクト処理の準備
    postEffectAudioSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// オーディオデバイスからのコールバックを受けて
// 指定されたオーディオバッファに音声データを書き込む関数
// prepareToPlay() の呼び出し以降、オーディオデバイスが新しい
// データブロックを必要とするたびにこのコールバックが実行される
// bufferToFill: 書き込み先バッファへのポインタと、書き込むべき範囲
//               （開始位置とサンプル数）をまとめた構造体
//               この範囲に書き込んだデータがオーディオデバイスへ転送される
void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // アクティブなバッファ領域をクリアする（ノイズ防止）
    bufferToFill.clearActiveBufferRegion();

    // 変更前: transportSource->getNextAudioBlock(bufferToFill);

    // オーディオ処理チェーンの最後のオーディオソースに
    // 書き込み先のオーディオバッファを渡す
    // オーディオ処理チェーンの処理結果を、このブロックで
    // 再生する音声データとして書き込んでもらう
    postEffectAudioSource->getNextAudioBlock(bufferToFill);
}

// オーディオリソースを解放する関数
void MainComponent::releaseResources()
{
    // 変更前: transportSource->releaseResources();

    // オーディオエフェクト処理のリソースを解放
    postEffectAudioSource->releaseResources();
}

//==============================================================================
// コンポーネントの描画を行う関数
// 背景の塗りつぶしと、その上に重ねる枠やテキストを描画する
// g: 描画に使用するグラフィックスコンテキスト
void MainComponent::paint (juce::Graphics& g)
{
    // コンポーネントの背景を不透明な色で塗りつぶす
    // 色は LookAndFeel が持つウインドウ背景色を使う
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // ユーザーガイダンステキストの枠を描画する
    // ドラッグ＆ドロップ可能なエリアであることを視覚的に示す
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(rectTextMessage.reduced(8, 8).toFloat(), 8.0f, 2.0f);
    
    // 音声ファイルドラッグ＆ドロップの説明テキストを描画する
    // 受け付ける形式は実行環境によって変わるため、テキストには含めない
    const juce::String messageGuide = "Drag and drop an audio file here";
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawFittedText(messageGuide, rectTextMessage, juce::Justification::centred, 2);

    // 現在再生中の音声ファイル名を表示する
    // ファイルが読み込まれている場合、ファイル名を追加で表示する
    if (currentPlayingAudioFile.getFullPathName().isNotEmpty())
    {
        const juce::String messageFile = "Playing: " + currentPlayingAudioFile.getFileName();
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawFittedText(messageFile, rectTextMessage.withY(rectTextMessage.getY() + 28.0f),
            juce::Justification::centred, 2);
    }
}

// コンポーネントのサイズが変更されたときに呼ばれる関数
void MainComponent::resized()
{
    // このコンポーネント全体のローカルな矩形領域を取得する
    auto rectLocal = getLocalBounds();

    // エフェクトコントロールを下部に配置する
    postEffectController->setBounds(rectLocal.removeFromBottom(240));

    // 波形表示をその上に配置する
    waveformView->setBounds(rectLocal.removeFromBottom(240));

    // 残りの領域をメッセージ表示に使用する
    rectTextMessage = rectLocal;

    // GUI の再描画を要求する
    repaint();
}

//==============================================================================
// 音声ファイルを読み込む関数
// fileToLoad: 読み込む音声ファイル
void MainComponent::loadAudioFile(const juce::File& fileToLoad)
{
    // 新しいファイルを読み込む前に、トランスポートソースの再生を停止する
    transportSource->stop();

    // トランスポートソースが保持している以前のオーディオソースを解除する
    transportSource->setSource(nullptr);

    // 既存の音声ファイルソースをリセットする
    audioFormatSource.reset();

    // 新しいファイルを読み込む
    if (auto* reader = audioFormatManager.createReaderFor(fileToLoad))
    {
        // 音声ファイルリーダーを使用して新しいオーディオソースを作成する
        audioFormatSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        
        // プリフェッチ用のバッファサイズを設定する定数
        // スムーズな再生のために、先読みするオーディオデータのサイズを定義する
        const int readerAheadSize = 32768;

        // トランスポートソースに音声サンプル読み込み対象のオーディオソースをセットする
        // 第四引数には音声ファイルの実際のサンプリングレートを渡す
        // 異なる値を渡すと再生速度やピッチが変化する原因になるので注意する
        transportSource->setSource(
            audioFormatSource.get(),     // 音声サンプル読み込み対象のオーディオソース
            readerAheadSize,             // プリフェッチ用のバッファサイズ
            audioBufferingThread.get(),  // バッファリングスレッド
            reader->sampleRate           // 読み込み対象のサンプリングレート
        );

        // ファイル読み込み後にトランスポートソースの再生を開始する
        transportSource->start();

        // 現在再生しているファイル名を表示するためのファイルオブジェクトを更新する
        // GUI に表示する現在のファイル情報を設定する
        currentPlayingAudioFile = fileToLoad;

        // 波形表示のための読み込みオブジェクトを生成して渡す
        audioThumbnail->setSource(new juce::FileInputSource(fileToLoad));
    }
    else
    {
        // ファイルの読み込みに失敗した場合、再生ファイル情報をクリアする
        currentPlayingAudioFile = "";
        
        // 音声波形サムネイルのデータを解放する
        audioThumbnail->setSource(nullptr);
    }

    // 新しいファイル情報や状態を反映するため、GUI の再描画を要求する
    repaint();
}

void MainComponent::showAudioDeviceSettingsDialog()
{
    juce::AudioDeviceSelectorComponent* selector =
        new juce::AudioDeviceSelectorComponent(deviceManager,
                                                0, 256,
                                                0, 256,
                                                true, true,
                                                true, false);
    selector->setSize(400, 600);

    juce::DialogWindow::LaunchOptions dialog;
    dialog.content.setOwned(selector);
    dialog.dialogTitle = "Audio/MIDI Device Settings";
    dialog.componentToCentreAround = this;
    dialog.dialogBackgroundColour =
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    dialog.escapeKeyTriggersCloseButton = true;
    dialog.useNativeTitleBar = false;
    dialog.resizable = false;
    dialog.useBottomRightCornerResizer = false;
    dialog.launchAsync();
}

//==============================================================================
// ドラッグされたファイルを受け入れるかどうかを判定する関数
// files: ドラッグされたファイルの配列
// 戻り値: 受け入れる場合は true、そうでない場合は false
bool MainComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    // オーディオフォーマットマネージャが認識できるファイル形式のフィルタを作成する
    const juce::WildcardFileFilter filter(
        audioFormatManager.getWildcardForAllFormats(), {}, "Known Audio Formats");

    // ファイルが1つだけで、そのファイルがフィルタに適合する場合に true を返す
    return files.size() == 1 && filter.isFileSuitable(files[0]);
}

// ファイルがドロップされたときに呼ばれる関数
// files: ドロップされたファイルの配列
// x, y: ドロップされた位置の座標
void MainComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    // ファイルが1つ以上ドロップされたことを判定する
    if (files.size() > 0)
    {
        // 最初のファイルのパスを取得する
        const juce::String filePath = files.getReference(0);

        // ファイルパスから juce::File オブジェクトを作成する
        const juce::File fileToLoad = juce::File(filePath);

        // 音声ファイルを読み込んで再生を開始する
        loadAudioFile(fileToLoad);
    }
}
