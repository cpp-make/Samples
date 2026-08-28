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
    addAndMakeVisible(m_volumeSlider);
    m_volumeSliderAttachment.reset(new SliderAttachment(audioProcessor.apvts, "volume", m_volumeSlider));
    
    addAndMakeVisible(m_typeComboBox);
    m_typeComboBox.addItemList(p.apvts.getParameter("type")->getAllValueStrings(), 1);
    m_typeComboBoxAttachment.reset(new ComboBoxAttachment(audioProcessor.apvts, "type", m_typeComboBox));
    
    addAndMakeVisible(m_midiKeyboardComponent);

    setSize (400, 300);
    
    startTimerHz(30);
}

DemoSynthesiserAudioProcessorEditor::~DemoSynthesiserAudioProcessorEditor()
{
    // アタッチメントは紐づけているコンポーネントより先にデストラクタが呼ばれる必要がある。
    // そのためここで明示的に解放する。
    m_volumeSliderAttachment = nullptr;
    m_typeComboBoxAttachment = nullptr;
}

//==============================================================================
void DemoSynthesiserAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    auto area = getLocalBounds().toFloat();
    area = area.removeFromTop(200.0f).removeFromRight(200.0f);

    // 描画エリアを表す枠線描画
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawRect(area);

    // 各chの音声波形を表すPathを描画する。
    g.setColour(juce::Colours::white);
    juce::Path path;
    const float halfHeight = area.getHeight() / 2;
    for (int ch = 0; ch < 2; ch++)
    {
        auto chArea = area.withTrimmedTop(halfHeight).withY(ch * halfHeight);
        
        for (int i = 0; i < m_stereoFifo.getSize(); ++i)
        {
            float x = juce::jmap((float)i, 0.0f, (float)(m_stereoFifo.getSize() - 1), chArea.getX(), chArea.getRight());
            float y = juce::jmap(m_stereoFifo.getReadPointer(ch)[i], -1.0f, 1.0f, chArea.getBottom(), chArea.getY());
            
            if (i == 0) path.startNewSubPath(x, y);
            else path.lineTo(x, y);
        }
    }
    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void DemoSynthesiserAudioProcessorEditor::resized()
{
    m_volumeSlider.setBounds(0, 0, 200, 40);
    m_typeComboBox.setBounds(0, 60, 200, 40);

    m_midiKeyboardComponent.setBounds(0, 200, 400, 100);
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
