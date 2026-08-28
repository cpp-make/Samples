#pragma once

#include "math.h"
#include "ray.h"
#include "real.h"
#include "vec.h"

struct Sphere
{
    // 球の半径
    Float radius;
    // 球の中心位置
    Vec3 position;
    // 球のマテリアルID
    uint32_t materialID;

    Sphere(const Float radius, const Vec3 &position, uint32_t materialID)
        : radius(radius), position(position), materialID(materialID)
    {
    }

    // 球とレイとの交差判定を行い、交差があればレイの交点までの距離を更新する
    // 戻り値がtrueなら交差しており、falseなら交差していない
    bool checkIntersectionAndUpdateRay(Ray &ray) const
    {
        // 交差判定前のレイの交点までの距離
        const Float originalDistance = ray.distanceToIntersection;
        // レイの原点から球の中心までのベクトル
        const Vec3 relativePosition = position - ray.origin;

        // 二次方程式を解いて、レイと球の交差判定を行う
        const Float b = Dot(relativePosition, ray.direction);
        const Float c = Dot(relativePosition, relativePosition) - radius * radius;
        if (const auto roots = SolveQuadratic(-b, c))
        {
            // 計算誤差を考慮した微小値
            const Float EPSILON = 1e-6;
            // 解のペア
            const auto t = *roots;

            // t[0]とt[1]が有効な交点であるかチェック
            if (EPSILON < t[0] && t[0] < ray.distanceToIntersection)
            {
                ray.distanceToIntersection = t[0];
            }
            if (EPSILON < t[1] && t[1] < ray.distanceToIntersection)
            {
                ray.distanceToIntersection = t[1];
            }
        }

        // 交差があればtrueを返す
        return (originalDistance > ray.distanceToIntersection);
    }

    // 指定された球上の点における法線ベクトルを計算する
    Vec3 computeNormal(const Vec3 &surfacePosition) const
    {
        // 球上の点から球の中心へのベクトルを正規化して返す
        return Normalize(surfacePosition - position);
    }
};
