/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DemoSynthesiserAudioProcessorEditor::DemoSynthesiserAudioProcessorEditor (DemoSynthesiserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
    , m_midiKeyboardComponent(audioProcessor.midiKeyboardState, juce::MidiKeyboardComponent::Orientation::horizontalKeyboard)
    , m_stereoFifo(p.stereoFifo)
{
    auto setupSlider = [this](juce::Slider& slider)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 24);
    };
    setupSlider(m_volumeSlider);
    m_volumeSliderAttachment.reset(new SliderAttachment(audioProcessor.apvts, "volume", m_volumeSlider));
    
    addAndMakeVisible(m_typeComboBox);
    m_typeComboBox.addItemList(p.apvts.getParameter("type")->getAllValueStrings(), 1);
    m_typeComboBoxAttachment.reset(new ComboBoxAttachment(audioProcessor.apvts, "type", m_typeComboBox));
    
    setupSlider(m_phaseSlider);
    m_phaseSliderAttachment.reset(new SliderAttachment(audioProcessor.apvts, "phase", m_phaseSlider));
    
    addAndMakeVisible(m_midiKeyboardComponent);
    
    auto setupLabel = [this](juce::Label& label, const juce::String& text, float fontHeight)
    {
        addAndMakeVisible(label);
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(fontHeight));
    };
    setupLabel(m_titleLabel, "DEMO SYNTHESISER", 24);
    setupLabel(m_volumeLabel, "VOLUME", 16);
    setupLabel(m_typeLabel, "TYPE", 16);
    setupLabel(m_phaseLabel, "PHASE", 16);

    // カラースキーム指定
    if (auto* lookAndFeel = dynamic_cast<juce::LookAndFeel_V4*>(&getLookAndFeel()))
    {
        using ColourScheme = juce::LookAndFeel_V4::ColourScheme;
        ColourScheme colourScheme = juce::LookAndFeel_V4::getMidnightColourScheme();
        colourScheme.setUIColour(ColourScheme::UIColour::menuBackground, juce::Colour(0xFF2D2D2F));
        colourScheme.setUIColour(ColourScheme::UIColour::menuText, juce::Colours::lightgrey);
        lookAndFeel->setColourScheme(colourScheme);
    }
    
    setSize (400, 500);

    startTimerHz(30);
}

DemoSynthesiserAudioProcessorEditor::~DemoSynthesiserAudioProcessorEditor()
{
    // アタッチメントは紐づけているコンポーネントより先にデストラクタが呼ばれる必要がある。
    // そのためここで明示的に解放する。
    m_volumeSliderAttachment = nullptr;
    m_typeComboBoxAttachment = nullptr;
    m_phaseSliderAttachment = nullptr;
}

//==============================================================================
void DemoSynthesiserAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // 描画エリアを表す枠線描画
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawRect(m_lissajousArea);

    // 各音声サンプルごとに1chをX位置,2chをY位置として描画する。
    for (int i = 0; i < m_stereoFifo.getSize(); ++i)
    {
        g.setColour(juce::Colours::white.withAlpha((float)i / (float)m_stereoFifo.getSize()));
        float x = juce::jmap(m_stereoFifo.getReadPointer(0)[i], -1.0f, 1.0f, m_lissajousArea.getX(), m_lissajousArea.getRight());
        float y = juce::jmap(m_stereoFifo.getReadPointer(1)[i], -1.0f, 1.0f, m_lissajousArea.getBottom(), m_lissajousArea.getY());
        g.fillEllipse(x, y, 1, 1);
    }
}

void DemoSynthesiserAudioProcessorEditor::resized()
{
    // 各コンポーネントのレイアウトを行う
    // 画面下部に表示するコンポーネントから表示範囲指定を行っている
    auto area = getLocalBounds();
    m_midiKeyboardComponent.setBounds(area.removeFromBottom(50));
    area.removeFromBottom(10);

    // 各パラメータのレイアウト
    // juce::CoomboBoxのみ高さ調整を行う
    auto paramGroupArea = area.removeFromBottom(150);
    paramGroupArea.reduce(50, 0);
    const int paramWidth = paramGroupArea.getWidth() / 3;
    auto layoutParam = [&](juce::Component* label, juce::Component* widget, bool isComboBox = false)
    {
        juce::Rectangle<int> paramArea = paramGroupArea.removeFromLeft(paramWidth).reduced(5);
        label->setBounds(paramArea.removeFromTop(25));
        if (isComboBox) paramArea = paramArea.withSizeKeepingCentre(paramArea.getWidth(), 25);
        widget->setBounds(paramArea);
    };
    layoutParam(&m_typeLabel, &m_typeComboBox, true);
    layoutParam(&m_phaseLabel, &m_phaseSlider);
    layoutParam(&m_volumeLabel, &m_volumeSlider);
    
    m_titleLabel.setBounds(area.removeFromBottom(50));

    const int visualizeSize = std::min(area.getWidth(), area.getHeight());
    m_lissajousArea = area.withSizeKeepingCentre(visualizeSize, visualizeSize).toFloat();
}

bool DemoSynthesiserAudioProcessorEditor::keyPressed (const juce::KeyPress& key)
{
    //  juce::ModifierKeys::commandModifierはmacならcommandキー、windowsならctrlキーとして扱われる。
    
    // command(ctrl)+zキーでundo(元に戻す)
    if (key == juce::KeyPress('z', juce::ModifierKeys::commandModifier, 0))
    {
        audioProcessor.undoManager.undo();
        return true;
    }

    // command(ctrl)+shift+zでredo(やり直し)
    if (key == juce::KeyPress('z', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier, 0))
    {
        audioProcessor.undoManager.redo();
        return true;
    }

    return juce::Component::keyPressed(key);
}

void DemoSynthesiserAudioProcessorEditor::timerCallback()
{
    // FIFO読み込み用データの更新。
    m_stereoFifo.readFromFifo();
    
    repaint();
}
