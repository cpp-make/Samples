#pragma once

#include <array>
#include <cstdint>

#include "sphere.h"
#include "material.h"

// レンダリングするシーンデータ

// シーン内のマテリアルを定義する配列
inline const std::array g_sceneMaterials = {
    Material(Color(1, 1, 1), Color(), Color(), 1.0),
    Material(Color(), Color(1, 1, 1), Color(), 1.0),
    Material(Color(), Color(1, 0, 0), Color(), 1.0),
    Material(Color(), Color(0, 1, 0), Color(1, 1, 1), 0.5),
    Material(Color(), Color(), Color(1, 1, 1), 0.0),
};

// シーン内の球体を定義する配列
inline const std::array g_sceneSpheres = {
    // 中心に配置された球
    Sphere(4, Vec3(0, 4, 0), 0 /* g_sceneMaterials のインデックス */),
    // 中心から右に配置された球
    Sphere(4, Vec3(8, 4, 0), 2),
    // 中心から左に配置された球
    Sphere(4, Vec3(-8, 4, 0), 3),
    // 床として機能する大きな球
    Sphere(5000, Vec3(0, -5000, 0), 1),
    // 奥の壁として機能する大きな球
    Sphere(5000, Vec3(0, 0, -5004), 1)};

inline int GetSceneSphereCount()
{
    return size(g_sceneSpheres);
}

inline const Sphere &GetSceneSphere(int index)
{
    if (0 <= index && index < GetSceneSphereCount())
    {
        return g_sceneSpheres[index];
    }
    // NOTE: 本当は範囲外エラーを出すとよい
    return g_sceneSpheres[0];
}

inline int GetSceneMaterialCount()
{
    return size(g_sceneMaterials);
}

inline const Material &GetSceneMaterial(int index)
{
    if (0 <= index && index < GetSceneMaterialCount())
    {
        return g_sceneMaterials[index];
    }
    // NOTE: 本当は範囲外エラーを出すとよい
    return g_sceneMaterials[0];
}

// シーンとレイの交差判定を行い、交差があればレイの情報を更新する関数
inline bool CheckSceneIntersectionAndUpdateRay(Ray &ray)
{
    // 交差判定前のレイの情報を保存しておく
    const Float originalDistance = ray.distanceToIntersection;
    const uint32_t originalPrimitiveID = ray.intersectedPrimitiveID;
    const int sphereCount = GetSceneSphereCount();

    // 全球に対して交差判定を行う
    for (int primitiveID = 0; primitiveID < sphereCount; ++primitiveID)
    {
        // 自己交差を避ける
        if (originalPrimitiveID == primitiveID)
        {
            continue;
        }

        // 交差判定とレイの更新
        if (GetSceneSphere(primitiveID).checkIntersectionAndUpdateRay(ray))
        {
            // 交差した球のIDをレイに設定
            ray.intersectedPrimitiveID = primitiveID;
        }
    }

    // 交差があればtrueを返す
    return (originalDistance > ray.distanceToIntersection);
}
