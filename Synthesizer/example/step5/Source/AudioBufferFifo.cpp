#include "AudioBufferFifo.h"

AudioBufferFifo::AudioBufferFifo(int numChannels, int fifoSize)
: m_numChannels(numChannels)
, m_abstractFifo(fifoSize)
{
    // 各配列のサイズを[ch数][FIFOサイズ]と同じにして値を全て0にする。
    m_fifoData.assign(numChannels, std::vector<float>(fifoSize, 0));
    m_readData.assign(numChannels, std::vector<float>(fifoSize, 0));
}

void AudioBufferFifo::writeToFifo(const juce::AudioBuffer<float>& inputBuffer)
{
    // FIFOに書き込むためのスペースが無い場合はスキップする。
    if (m_abstractFifo.getFreeSpace() == 0) return;

    const int numInputSamples = inputBuffer.getNumSamples();
    const int numInputChannels = inputBuffer.getNumChannels();

    // FIFOから最大numInputSamples分書き込む場合の配列のインデックスとサイズがそれぞれ二種類返される。
    // 関数スコープから抜けるタイミングで内部のインデックスが更新される。
    const auto scope = m_abstractFifo.write(numInputSamples);

    for (int ch = 0; ch < m_numChannels; ++ch)
    {
        const int inputCh = juce::jlimit(0, numInputChannels - 1, ch);
        const float* readBuf = inputBuffer.getReadPointer(inputCh);
        // FIFO配列末尾までの書き込み処理
        if (scope.blockSize1 > 0)
        {
            juce::FloatVectorOperations::copy(m_fifoData[ch].data() + scope.startIndex1,
                                              readBuf,
                                              scope.blockSize1);
        }
        // FIFO配列末尾まで書き込み済みで、配列先頭から折り返して書き込む必要がある場合の処理
        if (scope.blockSize2 > 0)
        {
            juce::FloatVectorOperations::copy(m_fifoData[ch].data() + scope.startIndex2,
                                              readBuf + scope.blockSize1,
                                              scope.blockSize2);
        }
    }
}

void AudioBufferFifo::readFromFifo()
{
    // FIFOに書き込みスペースがあり、満たされていない場合は書き込まれるまでスキップする。
    if (m_abstractFifo.getFreeSpace() != 0) return;
    
    // FIFOから読み込む場合の配列のインデックスとサイズがそれぞれ二種類返される。
    // 関数スコープから抜けるタイミングで内部のインデックスが更新される。
    const auto scope = m_abstractFifo.read(getSize());

    for (int ch = 0; ch < m_numChannels; ++ch)
    {
        // FIFO配列末尾までの読み込み処理
        if (scope.blockSize1 > 0)
        {
            juce::FloatVectorOperations::copy(m_readData[ch].data(),
                                              m_fifoData[ch].data() + scope.startIndex1,
                                              scope.blockSize1);
        }
        // FIFO配列末尾まで読み込み済みで、配列先頭から折り返して読み込む必要がある場合の処理
        if (scope.blockSize2 > 0)
        {
            juce::FloatVectorOperations::copy(m_readData[ch].data() + scope.blockSize1,
                                              m_fifoData[ch].data() + scope.startIndex2,
                                              scope.blockSize2);
        }
    }
}

int AudioBufferFifo::getSize() const
{
    return m_abstractFifo.getTotalSize();
}

const float* AudioBufferFifo::getReadPointer(int ch) const
{
    return m_readData[ch].data();
}
