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
{
    addAndMakeVisible(m_volumeSlider);
    m_volumeSliderAttachment.reset(new SliderAttachment(audioProcessor.apvts, "volume", m_volumeSlider));
    
    addAndMakeVisible(m_typeComboBox);
    m_typeComboBox.addItemList(p.apvts.getParameter("type")->getAllValueStrings(), 1);
    m_typeComboBoxAttachment.reset(new ComboBoxAttachment(audioProcessor.apvts, "type", m_typeComboBox));
    
    addAndMakeVisible(m_midiKeyboardComponent);

    setSize (400, 300);
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
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
