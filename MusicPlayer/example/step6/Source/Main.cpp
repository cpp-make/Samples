/*
  ==============================================================================

    JUCEアプリケーションの起動コード

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
// アプリケーションクラス: juce::JUCEApplication を継承して基本動作を実装
class MusicPlayerApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    // コンストラクタ：アプリケーション起動時の初期化を行う処理
    MusicPlayerApplication() {}

    // アプリケーションの基本情報を返す関数群
    // アプリケーション名
    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    // バージョン番号
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    // true の場合は複数起動を許可する
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    // アプリケーション初期化時に呼ばれる
    void initialise (const juce::String& commandLine) override
    {
        // アプリケーションウインドウを作成する
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    // アプリケーション終了時に呼ばれる
    void shutdown() override
    {
        // ウインドウインスタンスを破棄する
        mainWindow = nullptr;
    }

    //==============================================================================
    // OS からの終了要求を処理する
    void systemRequestedQuit() override
    {
        // ここで quit()を呼ぶと継承元のクラスにあるアプリケーション終了処理を実行する
        // 必要に応じて終了処理を実行しないことも可能
        quit();
    }

    // 別のインスタンスが起動された場合に呼ばれる
    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // 2つ目以降のインスタンスが起動された場合の処理
        // commandLine に起動時の引数が渡される
    }

    //==============================================================================
    /*
        メインウインドウクラス
        juce::DocumentWindow を継承して、MainComponentオブジェクトを内部に配置する
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        // コンストラクタ
        // 基底クラスの引数付きコンストラクタを呼び出す
        // 基底クラスのコンストラクタの第一引数: コンポーネント名
        // 基底クラスのコンストラクタの第二引数: ウインドウ内の背景色
        // 基底クラスのコンストラクタの第三引数: 最小化・最大化・閉じるボタンの設定
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                  .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            // ウインドウのタイトルバーのデザインに OSネイティブのものを使用する
            setUsingNativeTitleBar (true);

            // MainComponent をウインドウ内のコンポーネントとして配置する
            setContentOwned (new MainComponent(), true);

            // メニューバーの参照をこのウインドウに適用する
            setMenuBar(dynamic_cast<MainComponent*>(getContentComponent())->getMenuBarModel());

           #if JUCE_IOS || JUCE_ANDROID
            // モバイル向けは全画面表示する
            setFullScreen (true);
           #else
            // デスクトップ向けはリサイズ可能にする
            setResizable (true, true);

            // ウインドウをデスクトップ中央に配置する
            centreWithSize (getWidth(), getHeight());
           #endif

            // ウインドウを表示する
            setVisible (true);
        }

        // ウインドウの閉じるボタンが押されたときの処理
        void closeButtonPressed() override
        {
            // メニューバーの参照をこのウインドウから解放する
            setMenuBar(nullptr);

            // アプリケーションを終了する
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        // このマクロには2つの機能があります：
        // 1. クラスのコピーを禁止（コピーコンストラクタと代入演算子を削除）
        // 2. デバッグビルド時のメモリリーク検出機能の追加
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    // メインウインドウのインスタンス
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// プログラムのメインルーチンをクロスプラットフォームに生成するマクロ
START_JUCE_APPLICATION (MusicPlayerApplication)
