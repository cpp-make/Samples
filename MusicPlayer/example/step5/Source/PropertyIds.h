/*
  ==============================================================================

    PropertyIds.h
    Created: 2 Mar 2025 1:20:18pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#pragma once

// JUCEライブラリの統合ヘッダーをインクルードする
#include <JuceHeader.h>

// アプリケーション全体で共有するプロパティ識別子の名前空間
namespace PropertyId
{
// 再生制御関連のプロパティ識別子の名前空間
namespace Transport
{
// 再生可能状態（音声ファイルが読み込まれているか）を示すプロパティ識別子
extern const juce::Identifier CanPlay;

// 現在の再生状態（再生中かどうか）を示すプロパティ識別子
extern const juce::Identifier IsPlaying;

// ループ再生モードの有効/無効を示すプロパティ識別子
extern const juce::Identifier IsLooping;
}
}
