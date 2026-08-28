#pragma once

#include <variant>

#include "math.h"
#include "real.h"
#include "utility.h"
#include "vec.h"

class Material
{
public:
    Material(const Color &emissionColor, const Color &diffuseColor, const Color &mirrorColor,
             Float diffuseRatio)
        : m_emissionColor(emissionColor), m_diffuseColor(diffuseColor), m_mirrorColor(mirrorColor),
          m_diffuseRatio(diffuseRatio)
    {
    }

    // ディフューズマテリアルにおける、次の反射方向をサンプリングする関数
    Ray sampleNextDiffuseDirection(const Vec3 &surfacePosition, const Vec3 &surfaceNormal,
                                   const Ray &ray, Float randomValue0, Float randomValue1) const
    {
        // 交点の法線（物体からのレイの入出を考慮）
        const Vec3 orientingNormal =
            Dot(surfaceNormal, ray.direction) < 0 ? surfaceNormal : (-surfaceNormal);
        // コサイン項を使った重点的サンプリング
        const OrthonormalBasis orthoNormalBasis(orientingNormal);
        const Vec3 nextRayDirection =
            orthoNormalBasis.toWorldSpace(SampleCosWeightedHemisphere(randomValue0, randomValue1));

        const Vec3 nextRayOrigin = surfacePosition;
        return Ray(nextRayOrigin, nextRayDirection, ray.intersectedPrimitiveID);
    }

    // ミラーマテリアルにおける、次の反射方向をサンプリングする関数
    Ray sampleNextMirrorDirection(const Vec3 &surfacePosition, const Vec3 &surfaceNormal,
                                  const Ray &ray, Float randomValue0, Float randomValue1) const
    {
        const Vec3 nextRayDirection = Reflect(ray.direction, surfaceNormal);
        const Vec3 nextRayOrigin = surfacePosition;
        return Ray(nextRayOrigin, nextRayDirection, ray.intersectedPrimitiveID);
    }

    Color getEmissionColor() const
    {
        return m_emissionColor;
    }

    Color getDiffuseColor() const
    {
        return m_diffuseColor;
    }

    Color getMirrorColor() const
    {
        return m_mirrorColor;
    }

    Float getDiffuseRatio() const
    {
        return m_diffuseRatio;
    }

private:
    Color m_emissionColor;
    Color m_diffuseColor;
    Color m_mirrorColor;
    Float m_diffuseRatio;
};
