/*
  ==============================================================================

    MainMenuBarModel.cpp
    Created: 2 Mar 2025 9:20:05pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#include "MainMenuBarModel.h"

// コンストラクタの実装 - MainComponent のポインタを受け取り SafePointer に格納
MainMenuBarModel::MainMenuBarModel(juce::Component::SafePointer<MainComponent> mainComponent)
    : mainComponentPtr(mainComponent) // SafePointer によってコンポーネントを安全に参照
{
}

// デストラクタの実装
MainMenuBarModel::~MainMenuBarModel()
{
    // SafePointer は自動的にクリアされるため、特別な後処理は不要
}

// メニューバーの最上位メニュー名を返す関数
juce::StringArray MainMenuBarModel::getMenuBarNames()
{
    // 単一の「Menu」メニューを定義する
    return { "Menu" };
}

// 特定のメニューの内容を構築して返す関数
juce::PopupMenu MainMenuBarModel::getMenuForIndex(int topLevelMenuIndex,
    const juce::String& menuName)
{
    juce::ignoreUnused(menuName);

    // 新しいポップアップメニューのオブジェクトを作成する
    juce::PopupMenu menu;

    if (topLevelMenuIndex == 0)
    {
        // "Menu"メニューの場合

        // オーディオデバイス設定項目を追加
        menu.addItem(CommandIDs::kShowAudioDeviceSettings, "Show Audio Device Settings");

        // 区切り線を追加
        menu.addSeparator();

        // 音声ファイル選択項目を追加
        menu.addItem(CommandIDs::kLoadAudioFile, "Load Audio File");
    }

    // 構築したメニューを返す
    return menu;
}

// メニュー項目が選択されたときの処理を行う関数
void MainMenuBarModel::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    if (topLevelMenuIndex == 0)  // "Menu"メニューの場合
    {
        // "Show Audio Device Settings"が選択された場合
        if (menuItemID == CommandIDs::kShowAudioDeviceSettings)
        {
            // SafePointer から MainComponent を取得し、nullptr でないことを確認する
            if (MainComponent* component = mainComponentPtr.getComponent())
            {
                // オーディオデバイス設定ダイアログを表示する
                component->showAudioDeviceSettingsDialog();
            }
        }
        else if (menuItemID == CommandIDs::kLoadAudioFile)  // "Load Audio File"が選択された場合
        {
            // SafePointer から MainComponent を取得し、nullptr でないことを確認する
            if (MainComponent* component = mainComponentPtr.getComponent())
            {
                // ファイル選択ダイアログを表示する
                component->openAudioFileChooser();
            }
        }
    }
}
