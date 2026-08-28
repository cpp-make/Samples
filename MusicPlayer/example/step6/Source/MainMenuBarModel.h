/*
  ==============================================================================

    MainMenuBarModel.h
    Created: 2 Mar 2025 9:20:05pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#pragma once

// JUCEライブラリの統合ヘッダーをインクルードする
#include <JuceHeader.h>

// MainComponent の参照をメンバ変数とするのでインクルードする
#include "MainComponent.h"

// アプリケーションのメニューバー機能を実装するクラス
// juce::MenuBarModel を継承：メニューバーの基本機能を提供する基底クラス
class MainMenuBarModel final
    : public juce::MenuBarModel
{
public:
    // メニュー項目のコマンドID を定義する列挙型
    enum CommandIDs
    {
        kShowAudioDeviceSettings = 0x01,  // オーディオデバイス設定ダイアログ表示コマンド
        kLoadAudioFile = 0x02,            // 音声ファイル読み込みコマンド
    };

    // コンストラクタ
    // 第一引数: MainComponent オブジェクトのポインタを SafePointer でラップしたものを受け取る
    // explicitキーワード: 暗黙の型変換を防止する
    explicit MainMenuBarModel(juce::Component::SafePointer<MainComponent> mainComponent);

    // デストラクタ
    ~MainMenuBarModel() override;

private:
    // juce::MenuBarModel からオーバーライドした関数

    // メニューバーに表示するメニュー名の配列を返す
    juce::StringArray getMenuBarNames() override;

    // 指定されたインデックスのメニュー内容を構築して返す
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex,
        const juce::String& menuName) override;

    // メニュー項目が選択されたときに呼び出される
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

    //==============================================================================
    // MainComponent への安全なポインタ
    // SafePointer: 参照先オブジェクトが削除されると自動的に nullptr になるスマートポインタ
    juce::Component::SafePointer<MainComponent> mainComponentPtr;

    // コピーコンストラクタと代入演算子を無効化し、メモリリーク検出機能を有効化
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMenuBarModel)
};
