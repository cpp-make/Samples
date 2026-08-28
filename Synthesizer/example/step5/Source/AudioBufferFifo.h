#pragma once
#include <JuceHeader.h>

class AudioBufferFifo
{
public:
    AudioBufferFifo(int numChannels, int fifoSize);
    ~AudioBufferFifo() = default;
    
    // 音声を内部FIFOバッファに追加する。
    // juce::AudioProcessor派生クラスのprocessBlock()などオーディオスレッド処理から呼ばれる想定。
    void writeToFifo(const juce::AudioBuffer<float>& inputBuffer);
    
    // 内部FIFOバッファを読み込み、取得用のデータを更新する。
    void readFromFifo();
    
    // 外部からの読み取り用データのサイズを返す。
    int getSize() const;
    
    // 外部からの読み込み用データのポインタを返す。readFromFifo()が呼ばれた後に同じスレッドで使用する想定。
    const float* getReadPointer(int ch) const;
    
private:
    int m_numChannels;                              // チャンネル数
    juce::AbstractFifo m_abstractFifo;              // FIFO操作の管理
    std::vector<std::vector<float>> m_fifoData;     // FIFO配列
    std::vector<std::vector<float>> m_readData;     // 外部からの読み込み用データ

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioBufferFifo)
};
