/*
  ==============================================================================

    PropertyIds.cpp
    Created: 2 Mar 2025 1:20:18pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#include "PropertyIds.h"

namespace PropertyId
{
namespace Transport
{
// 再生可能状態（音声ファイルが読み込まれているか）を示すプロパティ識別子の実体定義
const juce::Identifier CanPlay{ "Transport::CanPlay" };

// 現在の再生状態（再生中かどうか）を示すプロパティ識別子の実体定義
const juce::Identifier IsPlaying{ "Transport::IsPlaying" };

// ループ再生モードの有効/無効を示すプロパティ識別子の実体定義
const juce::Identifier IsLooping{ "Transport::IsLooping" };
}
}
