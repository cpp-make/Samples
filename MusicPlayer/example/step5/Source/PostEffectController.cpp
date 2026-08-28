/*
  ==============================================================================

    PostEffectController.cpp
    Created: 1 Mar 2025 8:15:50pm
    Author:  Tatsuya Shiozawa

  ==============================================================================
*/

#include "PostEffectController.h"

// コンストラクタ - GUIコンポーネントを初期化
PostEffectController::PostEffectController(PostEffectAudioSource& audioSource)
    : postEffectAudioSourceRef(audioSource)  // 制御対象のオーディオソースへの参照を保持
{
    // イコライザーコントロール用のグループ表示コンポーネントを初期化
    groupFilterGain = std::make_unique<juce::GroupComponent>();
    groupFilterGain->setText("Filter Gain");  // グループ表示のタイトルを設定
    // タイトル位置を中央に設定
    groupFilterGain->setTextLabelPosition(juce::Justification::centred);

    // グループ表示をこのコンポーネントの子コンポーネントとして追加して表示可能にする
    addAndMakeVisible(groupFilterGain.get());

    // 対象の PostEffectAudioSource からフィルタ数を取得
    const uint32_t numFilters = postEffectAudioSourceRef.getNumFilters();
    
    // 各フィルタバンドのコントロールを作成
    for (int filter_index = 0; filter_index < numFilters; filter_index++)
    {
        // フィルタの周波数を文字列に変換（例: "250Hz"）
        juce::String textFrequency =
            juce::String(postEffectAudioSourceRef.getFilterFrequency(filter_index), 0) + "Hz";

        // 周波数表示用のラベルを作成
        auto* label = labelFrequencyList.add(new juce::Label());
        label->setJustificationType(juce::Justification::centred);  // テキスト位置を中央に設定
        label->setText(textFrequency, juce::dontSendNotification);  // ラベルテキストを設定

        // ラベルをこのコンポーネントの子コンポーネントとして追加して表示可能にする
        addAndMakeVisible(label);

        // ゲイン調整用のスライダーを作成
        auto* slider = sliderFilterGainList.add(new juce::Slider(
            juce::Slider::SliderStyle::LinearVertical,  // 縦方向のスライダースタイル
            juce::Slider::TextEntryBoxPosition::TextBoxBelow  // 入力欄を下部に配置
        ));
        // スライダーの範囲を-12dB から+12dB（0.01dB単位）に設定
        slider->setRange(-12.0, 12.0, 0.01);
        slider->setTextValueSuffix("dB");     // 値の単位を dB として表示
        slider->setDoubleClickReturnValue(true, 0.0);  // ダブルクリックで 0dB（フラット）に戻る
        slider->setSkewFactorFromMidPoint(0.0);  // スライダーの中間点を0.0に設定
        
        // スライダー値が変更されたときのコールバック関数を設定
        slider->onValueChange =
            [safe_this = juce::Component::SafePointer(this), filter_index]() {
            // コンポーネントが有効であることを確認
            if (safe_this.getComponent() == nullptr)
            {
                return;
            }

            // スライダーの値を取得してオーディオエフェクトに設定
            if (juce::Slider* slider =
                safe_this->sliderFilterGainList.getUnchecked(filter_index))
            {
                const double value = slider->getValue();
                safe_this->postEffectAudioSourceRef.setFilterGainDecibel(filter_index, value);
            }
            };
        
        // スライダーをこのコンポーネントの子コンポーネントとして追加して表示可能にする
        addAndMakeVisible(slider);
    }

    // 出力ゲインコントロール用のグループコンポーネントを初期化
    groupOutputGain = std::make_unique<juce::GroupComponent>();
    groupOutputGain->setText("Output Gain");  // グループのタイトルを設定
    // タイトル位置を中央に設定
    groupOutputGain->setTextLabelPosition(juce::Justification::centred);

    // グループ表示をこのコンポーネントの子コンポーネントとして追加して表示可能にする
    addAndMakeVisible(groupOutputGain.get());

    // 出力ゲイン調整用のスライダーを作成
    sliderOutputGain = std::make_unique<juce::Slider>(
        juce::Slider::SliderStyle::LinearVertical,  // 縦方向のスライダースタイル
        juce::Slider::TextEntryBoxPosition::TextBoxBelow  // テキスト入力ボックスを下部に配置
    );
    // スライダーの範囲を-∞dB から+12dB（0.01dB単位）に設定
    sliderOutputGain->setRange(juce::Decibels::gainToDecibels(0.0f), 12.0, 0.01);
    sliderOutputGain->setTextValueSuffix("dB");  // 値の単位を dB として表示
    sliderOutputGain->setDoubleClickReturnValue(true, 0.0);  // ダブルクリックで 0dB に戻る
    // 中間点を-12.0dB に設定（低レベルでの微調整が容易になる）
    sliderOutputGain->setSkewFactorFromMidPoint(-12.0);
    
    // スライダー値が変更されたときのコールバック関数を設定
    sliderOutputGain->onValueChange =
        [safe_this = juce::Component::SafePointer(this)]() {
        // コンポーネントが有効であることを確認
        if (safe_this.getComponent() == nullptr)
        {
            return;
        }

        // スライダーの値を取得してオーディオエフェクトに設定
        const double value = safe_this->sliderOutputGain->getValue();
        safe_this->postEffectAudioSourceRef.setOutputGainDecibel(value);
        };

    // スライダーをこのコンポーネントの子コンポーネントとして追加して表示可能にする
    addAndMakeVisible(sliderOutputGain.get());
}

// デストラクタ
PostEffectController::~PostEffectController()
{
    // スマートポインタと juce::OwnedArray が自動的に破棄するので
    // 明示的な解放処理は省略する
}

// コンポーネントの描画を行う関数
void PostEffectController::paint(juce::Graphics& g)
{
    // 特に描画処理は不要（子コンポーネントが自身を描画する）
    juce::ignoreUnused(g);
}

// コンポーネントのサイズが変更されたときに呼ばれる関数
void PostEffectController::resized()
{
    // このコンポーネントのローカル座標系の矩形領域を取得する
    juce::Rectangle<int> rectLocal = getLocalBounds();

    // 左側80%をイコライザーコントロールに、残りを出力ゲインコントロールに割り当てる
    groupFilterGain->setBounds(
        rectLocal.removeFromLeft(rectLocal.getWidth() * 0.8f).reduced(8));
    {
        const int num_sliders = sliderFilterGainList.size();  // スライダーの数を取得する
        // グループ表示コンポーネントの内側の矩形を余白付きで取得する
        juce::Rectangle<int> rect_filter_gain_group = groupFilterGain->getBounds().reduced(8);
        // 内側の矩形の幅をバンド数で等分して、各スライダーの幅を求める
        const int width = rect_filter_gain_group.getWidth() / num_sliders;
        // 周波数ラベルの高さを、内側の矩形の高さの8分の1として求める
        const int height_frequency = rect_filter_gain_group.getHeight() / 8;

        // 各バンドのコントロールを左から順に配置する
        for (int slider_index = 0; slider_index < num_sliders; slider_index++)
        {
            // 左端から1バンド分の幅を切り出し、余白を除いた矩形を得る
            auto rect_slider = rect_filter_gain_group.removeFromLeft(width).reduced(4);
            // 切り出した矩形の上部に周波数ラベルを配置する
            labelFrequencyList.getUnchecked(slider_index)
                ->setBounds(rect_slider.removeFromTop(height_frequency));
            // ラベルを除いた残りの領域にスライダーを配置する
            sliderFilterGainList.getUnchecked(slider_index)->setBounds(rect_slider);
        }
    }

    // イコライザーコントロールに割り当てた残りの領域に出力ゲインコントロールを配置する
    groupOutputGain->setBounds(rectLocal.reduced(8));
    {
        // グループ表示コンポーネントの内側の矩形を余白付きで取得する
        juce::Rectangle<int> rect_slider = groupOutputGain->getBounds().reduced(8);

        // さらに余白を設定して出力ゲインスライダーを配置する
        sliderOutputGain->setBounds(rect_slider.reduced(4));
    }
}
