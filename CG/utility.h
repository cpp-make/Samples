#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "vec.h"

// 画像
class Image
{
public:
    Image(int width, int height) : m_width(width), m_height(height), m_imagePixels(width * height)
    {
    }
    Color getPixel(int x, int y) const
    {
        // NOTE: 範囲外チェックが必要ならしてください
        return m_imagePixels[x + y * m_width];
    }
    void setPixel(int x, int y, const Color &color)
    {
        // NOTE: 範囲外チェックが必要ならしてください
        m_imagePixels[x + y * m_width] = color;
    }
    int getWidth() const
    {
        return m_width;
    }
    int getHeight() const
    {
        return m_height;
    }

private:
    // 画像の解像度
    int m_width = 0;
    int m_height = 0;
    // 画像のピクセルデータを保持する
    std::vector<Color> m_imagePixels = {};
};

// 進捗バーを更新する関数
inline void UpdateProgressBar(int &currentProgress, int total)
{
    static std::mutex mtx;
    static auto startTime = std::chrono::high_resolution_clock::now();

    // スレッドセーフにするために、mutexを用いて関数全体をロックする
    std::lock_guard<std::mutex> guard(mtx);
    std::stringstream ss;

    // 進捗のパーセント表示
    ss << std::setw(4) << ((currentProgress + 1) * 100 / total) << " % ";

    const int BAR_WIDTH = 30;
    const int barPosition = BAR_WIDTH * currentProgress / total;

    ss << "|";
    for (int i = 0; i < BAR_WIDTH; ++i)
    {
        if (i <= barPosition)
        {
            ss << "=";
        }
        else
        {
            ss << " ";
        }
    }
    ss << "| ";

    // 現在までの時間と予測残り時間の計算
    const auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    const auto remainingTime =
        elapsedTime * (total - (currentProgress + 1)) / (currentProgress + 1);

    // 時間のフォーマットと出力
    const auto formatDuration = [](auto duration, std::stringstream &ss) {
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        int minutes = secs / 60;
        int seconds = secs % 60;
        ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0')
           << std::setw(2) << seconds << " ";
    };
    ss << "経過時間 ";
    formatDuration(elapsedTime, ss);
    ss << "予測残り時間 ";
    formatDuration(remainingTime, ss);

    // 出力の最後に改行を追加する
    if ((currentProgress + 1) != total)
    {
        // カーソルを行頭に戻す
        ss << "\r";
    }
    else
    {
        ss << "\n";
    }
    ++currentProgress;

    std::cout << ss.str();
}

// 並列実行を行うテンプレート関数
template <typename Fun> void ParallelFor(int numTasks, Fun &&fun)
{
    // NOTE: 以下は簡易的な実装
    // OpenMP、Intel TBB、std::execution::parなどの並列ライブラリの利用を検討すべし
    std::atomic<int> currentTaskIndex = 0;
    std::vector<std::thread> threads;
    // 利用可能なスレッド数（取得できなかった場合は1）
    const unsigned int numThreads = std::max(1u, std::thread::hardware_concurrency());

    for (unsigned int i = 0; i < numThreads; ++i)
    {
        const auto taskExecutor = [&currentTaskIndex, numTasks, &fun]() {
            for (;;)
            {
                const int taskIndex = currentTaskIndex.fetch_add(1);
                if (taskIndex >= numTasks)
                {
                    // すべてのタスクが終了したら終了
                    return;
                }
                // タスクを実行
                fun(taskIndex);
            }
        };
        threads.push_back(std::thread(taskExecutor));
    }

    // すべてのスレッドが終了するまで待機
    for (auto &thread : threads)
    {
        thread.join();
    }
}

// PNGとして画像保存
void SavePNG(const std::string &filename, const Image &image);
