#pragma once

#include <array>
#include <cstdint>

#include "material.h"
#include "sphere.h"

// 図18用のシーンデータ

inline const std::array g_sceneMaterials = {
    // 白いスタジオを照らす画面外の光源
    Material(Color(8.0, 7.7, 7.4), Color(), Color(), 1.0),
    // 黄色の発光マテリアル
    Material(Color(1.3, 0.9, 0.12), Color(), Color(), 1.0),
    Material(Color(1.3, 0.9, 0.12), Color(), Color(), 1.0),
    // 白い球と床の拡散反射マテリアル
    Material(Color(), Color(0.96, 0.94, 0.92), Color(), 1.0),
    Material(Color(), Color(0.90, 0.90, 0.90), Color(), 1.0),
    // 純粋な鏡面反射マテリアル
    Material(Color(), Color(), Color(1.0, 1.0, 1.0), 0.0),
};

inline const std::array g_sceneSpheres = {
    // 画面外の大きな面光源
    Sphere(4.5, Vec3(-14.0, 15.0, 12.0), 0),
    // 隣接する球同士が接するよう、奥から手前へ配置する
    Sphere(4.6, Vec3(-2.7, 4.6, -5.5), 3),
    Sphere(1.55, Vec3(5.0, 1.55, -0.4), 5),
    Sphere(2.55, Vec3(-6.0, 2.55, 0.644173), 3),
    Sphere(2.35, Vec3(0.3, 2.35, 3.556676), 3),
    // 白い球の間に置く黄色の発光球
    Sphere(1.6, Vec3(-3.565619, 1.6, 3.868113), 1),
    Sphere(1.02, Vec3(3.403767, 1.02, 4.397288), 1),
    Sphere(1.45, Vec3(-5.909578, 6.326558, -0.670909), 2),
    // 床。背景には無限遠の環境光を使う
    Sphere(5000.0, Vec3(0.0, -5000.0, 0.0), 4),
};

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
    return g_sceneMaterials[0];
}

inline bool CheckSceneIntersectionAndUpdateRay(Ray &ray)
{
    const Float originalDistance = ray.distanceToIntersection;
    const uint32_t originalPrimitiveID = ray.intersectedPrimitiveID;
    const int sphereCount = GetSceneSphereCount();

    for (int primitiveID = 0; primitiveID < sphereCount; ++primitiveID)
    {
        if (originalPrimitiveID == primitiveID)
        {
            continue;
        }

        if (GetSceneSphere(primitiveID).checkIntersectionAndUpdateRay(ray))
        {
            ray.intersectedPrimitiveID = primitiveID;
        }
    }

    return (originalDistance > ray.distanceToIntersection);
}
