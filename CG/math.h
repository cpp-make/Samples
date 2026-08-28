#pragma once

#include <array>
#include <cmath>
#include <numbers>
#include <optional>
#include <tuple>

#include "real.h"
#include "vec.h"

// 正規直交基底クラス
struct OrthonormalBasis
{
    // 正規直交既定のw, u, v軸を表すベクトル
    Vec3 w = {};
    Vec3 u = {};
    Vec3 v = {};

    OrthonormalBasis(const Vec3 &normal)
    {
        // 与えられた法線をw軸として設定
        w = normal;

        // 法線ベクトルのx成分とz成分を比較して、v軸を決定
        if (std::abs(normal.x) > std::abs(normal.z))
        {
            v = Vec3(-normal.y, normal.x, 0);
        }
        else
        {
            v = Vec3(0, -normal.z, normal.y);
        }
        // v軸を正規化
        v = Normalize(v);
        // u軸をv軸と法線ベクトルの外積として計算
        u = Cross(v, normal);
    }

    // ローカル空間のベクトルをワールド空間のベクトルに変換
    Vec3 toWorldSpace(const Vec3 &localSpaceVec) const
    {
        return Vec3(u * localSpaceVec.x + v * localSpaceVec.y + w * localSpaceVec.z);
    }
};

// 二次方程式の解の公式
// 0 = x^2 + 2 b x + c を解く
inline std::optional<std::array<Float, 2>> SolveQuadratic(Float b, Float c)
{
    // 判別式を計算
    const Float discriminant = b * b - c;
    if (0 > discriminant)
    {
        // 判別式が負の場合、解なし
        return std::nullopt;
    }
    const Float tmp = -b - std::copysign(std::sqrt(discriminant), b);
    // 二つの解を計算
    const Float x1 = c / tmp;
    const Float x2 = tmp;
    return std::array<Float, 2>{x1, x2};
}

// コサイン重み半球サンプリング
// 半球上にコサイン重み分布に従うランダムなベクトルを生成
inline Vec3 SampleCosWeightedHemisphere(Float randomValue0, Float randomValue1)
{
    const Float r0 = 2 * std::numbers::pi_v<Float> * randomValue0;
    const Float r1 = std::sqrt(randomValue1);
    const Vec3 tmp(std::cos(r0) * r1, std::sin(r0) * r1, std::sqrt(1 - randomValue1));
    return Normalize(tmp);
}
