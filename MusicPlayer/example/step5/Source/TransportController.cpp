/*
  ==============================================================================

    TransportController.cpp
    Created: 2 Mar 2025 1:20:10pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#include "TransportController.h"

// 状態管理ツリーのプロパティ識別子をインクルードする
#include "PropertyIds.h"

// 状態管理ツリーを受け取り、GUIコンポーネントを初期化するコンストラクタ
TransportController::TransportController(juce::ValueTree& appState)
    : applicationState(appState)  // 値コピーだが、内部データは共有オブジェクトである
{
    // グループコンポーネントの作成と初期設定
    group = std::make_unique<juce::GroupComponent>();
    group->setText("Transport");
    group->setTextLabelPosition(juce::Justification::centred);
    addAndMakeVisible(group.get());

    // 再生/停止ボタンの作成と初期設定
    playButton = std::make_unique<juce::TextButton>();
    playButton->setButtonText("Play");
    playButton->setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colour(0xFF2F9E44));
    playButton->onClick =
        [safe_this = juce::Component::SafePointer(this)]() {
        if (safe_this.getComponent() == nullptr)
        {
            return;
        }

        // ボタンクリック時に再生状態を反転させる
        safe_this->valueIsPlaying = !safe_this->valueIsPlaying.get();
        };
    addAndMakeVisible(playButton.get());

    // ループボタンの作成と初期設定
    loopButton = std::make_unique<juce::TextButton>();
    loopButton->setButtonText("Loop");
    loopButton->setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colour(0xFFFF7733));
    loopButton->onClick =
        [safe_this = juce::Component::SafePointer(this)]() {
        if (safe_this.getComponent() == nullptr)
        {
            return;
        }

        // ボタンクリック時にループ状態を反転させる
        safe_this->valueIsLooping = !safe_this->valueIsLooping.get();
        };
    addAndMakeVisible(loopButton.get());

    // 状態管理ツリーへリスナー登録
    applicationState.addListener(this);

    // 各状態値と ValueTree のプロパティを関連付け
    valueCanPlay.referTo(applicationState, PropertyId::Transport::CanPlay, nullptr);
    valueCanPlay.forceUpdateOfCachedValue();

    valueIsPlaying.referTo(applicationState, PropertyId::Transport::IsPlaying, nullptr);
    valueIsPlaying.forceUpdateOfCachedValue();

    valueIsLooping.referTo(applicationState, PropertyId::Transport::IsLooping, nullptr);
    valueIsLooping.forceUpdateOfCachedValue();

    // 初期状態を GUI に反映する
    updateView();
}

// デストラクタ
TransportController::~TransportController()
{
    // 状態管理ツリーのリスナー登録を解除する
    applicationState.removeListener(this);
}

// コンポーネントの描画を行う関数
void TransportController::paint(juce::Graphics& g)
{
    // 特に描画処理は不要（子コンポーネントが自身を描画する）
    juce::ignoreUnused(g);
}

// コンポーネントのサイズが変更されたときに呼ばれる関数
void TransportController::resized()
{
    // このコンポーネントのローカル座標系の矩形領域を取得する
    auto rectLocal = getLocalBounds();

    // 取得した矩形から余白を除いた領域にグループ表示コンポーネントを配置する
    group->setBounds(rectLocal.reduced(8));

    {
        // グループ表示コンポーネントが配置される矩形から余白分を削除した矩形を得る
        auto rect_group_inner = group->getBounds().reduced(8);

        // ボタンの幅と高さを算出する
        const auto width = rect_group_inner.getWidth();
        const auto height = rect_group_inner.getHeight() / 2;
        const auto width_button = width;
        const auto height_button = 60;

        // 再生ボタンの配置 - 上半分
        const juce::Rectangle<int> rect_play_button = 
            rect_group_inner.removeFromTop(height)
                .withSizeKeepingCentre(width_button, height_button);
        playButton->setBounds(rect_play_button.reduced(8));

        // ループボタンの配置 - 下半分
        const juce::Rectangle<int> rect_loop_button = 
            rect_group_inner.removeFromTop(height)
                .withSizeKeepingCentre(width_button, height_button);
        loopButton->setBounds(rect_loop_button.reduced(8));
    }
}

//==============================================================================
// juce::ValueTree のプロパティが変更された際に呼ばれるコールバック関数
void TransportController::valueTreePropertyChanged(
    juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& propertyId)
{
    if (treeWhosePropertyHasChanged == applicationState)
    {
        if (propertyId == valueCanPlay.getPropertyID())  // 再生可能状態が更新された場合
        {
            // juce::CachedValue の内部の値を更新する
            valueCanPlay.forceUpdateOfCachedValue();

            // 取得した状態値からボタンの有効/無効を切り替える
            playButton->setEnabled(valueCanPlay.get());
        }
        else if (propertyId == valueIsPlaying.getPropertyID())  // 再生状態が更新された場合
        {
            // juce::CachedValue の内部の値を更新する
            valueIsPlaying.forceUpdateOfCachedValue();

            // 取得した状態値からボタンのトグルON/OFF を切り替える
            playButton->setToggleState(valueIsPlaying.get(), juce::dontSendNotification);
        }
        else if (propertyId == valueIsLooping.getPropertyID())  // ループ状態が更新された場合
        {
            // juce::CachedValue の内部の値を更新する
            valueIsLooping.forceUpdateOfCachedValue();

            // 取得した状態値からボタンのトグルON/OFF を切り替える
            loopButton->setToggleState(valueIsLooping.get(), juce::dontSendNotification);
        }
    }
}

//==============================================================================
// GUIコンポーネントの表示状態を最新の状態値に基づいて更新する
void TransportController::updateView()
{
    // 再生ボタンの有効/無効状態を更新する
    playButton->setEnabled(valueCanPlay.get());

    // 再生ボタンのトグル状態を更新する
    playButton->setToggleState(valueIsPlaying.get(), juce::dontSendNotification);

    // ループボタンのトグル状態を更新する
    loopButton->setToggleState(valueIsLooping.get(), juce::dontSendNotification);
}
