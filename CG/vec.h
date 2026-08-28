#pragma once

#include <cmath>

#include "real.h"

template <typename Float> struct Vec3_t
{
    // ベクトルのx, y, z成分
    Float x = {};
    Float y = {};
    Float z = {};

    Vec3_t() = default;
    Vec3_t(Float x, Float y, Float z) : x(x), y(y), z(z)
    {
    }

    // 全成分を同じ値で初期化するコンストラクタ
    Vec3_t(Float initialValue) : Vec3_t(initialValue, initialValue, initialValue)
    {
    }

    Vec3_t operator-() const
    {
        return {-x, -y, -z};
    }
    Vec3_t operator+(const Vec3_t &rhs) const
    {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }
    Vec3_t operator-(const Vec3_t &rhs) const
    {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }
    Vec3_t operator*(const Vec3_t &rhs) const
    {
        return {x * rhs.x, y * rhs.y, z * rhs.z};
    }
    Vec3_t operator*(Float rhs) const
    {
        return {x * rhs, y * rhs, z * rhs};
    }
    Vec3_t operator/(Float rhs) const
    {
        return {x / rhs, y / rhs, z / rhs};
    }
    Vec3_t operator+=(const Vec3_t &rhs)
    {
        *this = *this + rhs;
        return *this;
    }
    Vec3_t operator*=(Float rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    Vec3_t operator/=(Float rhs)
    {
        *this = *this / rhs;
        return *this;
    }
};

using Vec3 = Vec3_t<Float>;

// ベクトルの長さ
inline Float Length(const Vec3 &v)
{
    return std::hypot(v.x, v.y, v.z);
}

// ベクトルの正規化
inline Vec3 Normalize(const Vec3 &v)
{
    return v / Length(v);
}

// ベクトルの内積
inline Float Dot(const Vec3 &v1, const Vec3 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// ベクトルの外積
inline Vec3 Cross(const Vec3 &v1, const Vec3 &v2)
{
    return {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z),
            (v1.x * v2.y) - (v1.y * v2.x)};
}

// 反射ベクトル
inline Vec3 Reflect(const Vec3 &direction, const Vec3 &normal)
{
    return direction - normal * 2 * Dot(normal, direction);
}

// 色を表すためにVec3を再利用する
using Color = Vec3_t<Float>;
