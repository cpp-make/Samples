#pragma once

#include <random>

#include "random_pcg.h"
#include "real.h"
#include "vec.h"

class Random
{
public:
    Random() = default;
    Random(uint32_t seed) : m_engine(seed)
    {
    }

    // 32bit の乱数を得る
    uint32_t get()
    {
        return m_engine();
    }

    // [0, 1) の乱数を得る
    Float getFloatInRange()
    {
        Float result;
        do
        {
            // 生成された乱数をUINT32_MAXで割ることで[0, 1)の範囲に正規化
            result = Float(m_engine()) / Float(UINT32_MAX);
        } while (result == Float(1)); // 極めてまれに1に一致してしまうので、その場合は再生成する
        return result;
    }

private:
    RandomNumberGeneratorWith_PCG_64_32 m_engine;
};
