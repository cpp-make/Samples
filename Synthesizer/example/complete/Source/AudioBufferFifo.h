/*
  ==============================================================================

    AudioBufferFifo.h
    Author:  migizo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class AudioBufferFifo
{
public:
    AudioBufferFifo(int numChannels, int fifoSize);
    ~AudioBufferFifo() = default;
    
    /// @brief 音声を内部FIFOバッファに追加する。juce::AudioProcessor派生クラスのprocessBlock()などオーディオスレッド処理から呼ばれる想定。
    /// @param inputBuffer FIFOバッファに追加する音声データ
    void writeToFifo(const juce::AudioBuffer<float>& inputBuffer);
    
    /// @brief 内部FIFOバッファを読み込み、取得用のデータを更新する。
    void readFromFifo();
    
    /// @brief 外部からの読み取り用データのサイズを返す。
    int getSize() const;
    
    /// @brief 外部からの読み込み用データのポインタを返す。readFromFifo()が呼ばれた後に同じスレッドで使用する想定。
    /// @param ch 読み込み対象チャンネル番号。
    const float* getReadPointer(int ch) const;
    
private:
    /// @brief チャンネル数
    int m_numChannels;
    /// @brief FIFO操作の管理
    juce::AbstractFifo m_abstractFifo;
    /// @brief FIFO配列
    std::vector<std::vector<float>> m_fifoData;
    /// @brief 外部からの読み込み用データ
    std::vector<std::vector<float>> m_readData;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioBufferFifo)
};
