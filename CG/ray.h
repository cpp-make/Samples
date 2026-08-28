#pragma once

#include <cstdint>
#include <limits>

#include "real.h"
#include "vec.h"

// レイ（光線）を表現する
struct Ray
{
    // 光線の原点
    Vec3 origin;
    // 光線の方向(正規化済み)
    Vec3 direction;
    // 交点までの距離
    Float distanceToIntersection;
    // 交差したプリミティブのID
    uint32_t intersectedPrimitiveID;

    // 有効なプリミティブIDが存在しないことを示す定数
    static constexpr uint32_t INVALID_PRIMITIVE_ID = std::numeric_limits<uint32_t>::max();
    // 交点までの距離がまだ決まっていないことを示す値
    static constexpr Float INFINITE_DISTANCE = std::numeric_limits<Float>::infinity();

    Ray(const Vec3 &origin, const Vec3 &direction,
        uint32_t intersectedPrimitiveID = INVALID_PRIMITIVE_ID,
        Float distanceToIntersection = INFINITE_DISTANCE)
        : origin(origin), direction(direction), distanceToIntersection(distanceToIntersection),
          intersectedPrimitiveID(intersectedPrimitiveID)
    {
    }

    // レイの方向と交点までの距離をもとに、交点の位置を計算する
    Vec3 getIntersectionPoint() const
    {
        return direction * distanceToIntersection + origin;
    }
};
