// Copyright(c) 2014 - 2017 Melissa O'Neill and PCG Project contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>

// PCG(64/32)を用いた乱数生成器
// http://www.pcg-random.org/download.html
// m_incrementのデフォルト値は公式実装のものを利用 
class RandomNumberGeneratorWith_PCG_64_32
{
public:
    using result_type = uint32_t;

    static constexpr result_type min() noexcept
    {
        return 0;
    }
    static constexpr result_type max() noexcept
    {
        return 0xffffffff;
    }

    RandomNumberGeneratorWith_PCG_64_32() noexcept
        : m_state(0x853c49e6748fea9bULL), m_increment(0xda3e39cb94b95bdbULL)
    {
    }

    RandomNumberGeneratorWith_PCG_64_32(uint64_t s) noexcept : RandomNumberGeneratorWith_PCG_64_32()
    {
        seed(s);
    }

    void seed(uint64_t seedValue) noexcept
    {
        m_state = Splitmix64(seedValue).next();
    }

    [[nodiscard]] result_type operator()() noexcept
    {
        uint64_t oldstate = m_state;
        m_state = oldstate * 6364136223846793005ULL + (m_increment | 1);
        uint32_t xorshifted = uint32_t(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = oldstate >> 59u;
        return rotr(xorshifted, rot);
    }

private:
    uint64_t m_state;
    uint64_t m_increment;

    static uint32_t rotr(uint32_t x, int shift)
    {
        return (x >> shift) | (x << (32 - shift));
    }
    static uint64_t rotr(uint64_t x, int shift)
    {
        return (x >> shift) | (x << (64 - shift));
    }

    // RandomNumberGeneratorWith_PCG_64_32::seed() で使うための、独立の乱数生成器
    struct Splitmix64
    {
        uint64_t state;

        Splitmix64(uint64_t seed = 0) noexcept : state(seed)
        {
        }

        [[nodiscard]] uint64_t next() noexcept
        {
            uint64_t z = (state += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            return z ^ (z >> 31);
        }
    };
};
