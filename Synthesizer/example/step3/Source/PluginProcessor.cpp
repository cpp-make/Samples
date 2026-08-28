/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // volumeをdB単位で扱うための変数
    auto volumeStringFromValue = [](auto v, int) -> juce::String { return juce::String(v, 2) + "dB"; };
    juce::NormalisableRange<float> volumeNoralisableRange(juce::Decibels::gainToDecibels(0.0f), 12.0f, 0.01f);
    volumeNoralisableRange.setSkewForCentre(-12.0f);
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("volume", 1),
                                                           "volume",
                                                           volumeNoralisableRange,
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes().withStringFromValueFunction(volumeStringFromValue)));

    return layout;
}

//==============================================================================
DemoSynthesiserAudioProcessor::DemoSynthesiserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
:
#endif
apvts(*this, &undoManager, "PARAMETERS", createLayout())
{
}

DemoSynthesiserAudioProcessor::~DemoSynthesiserAudioProcessor()
{
}

//==============================================================================
const juce::String DemoSynthesiserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DemoSynthesiserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DemoSynthesiserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DemoSynthesiserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DemoSynthesiserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DemoSynthesiserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DemoSynthesiserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DemoSynthesiserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DemoSynthesiserAudioProcessor::getProgramName (int index)
{
    return {};
}

void DemoSynthesiserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DemoSynthesiserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // シンセサイザー（音源部）のサンプリングレートを設定
    m_synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    
    resetSynthesiser();
    setupSynthesiser();
}

void DemoSynthesiserAudioProcessor::releaseResources()
{
    resetSynthesiser();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DemoSynthesiserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DemoSynthesiserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // midiMessagesが保持しているMIDIデータをスキャンし、midiKeyboardStateに追加する。
    // midiKeyboardStateが出力用に蓄積していたMIDIデータをmidiMessagesに追加する。
    midiKeyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);
    
    // MIDIを受け取り生成した音声をbufferに格納する。
    m_synthesiser.renderNextBlock(buffer, midiMessages, 0,  numSamples);
}

//==============================================================================
bool DemoSynthesiserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DemoSynthesiserAudioProcessor::createEditor()
{
    return new DemoSynthesiserAudioProcessorEditor (*this);
}

//==============================================================================
void DemoSynthesiserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // プロジェクトデータに書き込むためdestDataにパラメータ設定をコピーする
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void DemoSynthesiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // プロジェクトデータから読み込むためdataをXML形式にしてapvtsのパラメータ設定を置き換える
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
void DemoSynthesiserAudioProcessor::resetSynthesiser()
{
    m_synthesiser.clearVoices();
    m_synthesiser.clearSounds();
}

void DemoSynthesiserAudioProcessor::setupSynthesiser()
{
    for (int i = 0; i < 128; ++i)
    {
        m_synthesiser.addVoice(new SynthesiserVoice());
    }
    m_synthesiser.addSound(new SynthesiserSound(*apvts.getRawParameterValue("volume")));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DemoSynthesiserAudioProcessor();
}
