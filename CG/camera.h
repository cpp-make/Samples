#pragma once

#include <limits>

#include "ray.h"
#include "vec.h"

class Camera
{
public:
    Camera(int width, int height, const Vec3 &position, const Vec3 &direction,
           const Vec3 &up,
           // スクリーンの大きさ
           Float screenSize,
           // スクリーンまでの距離
           Float distanceToScreen)
        : m_width(width), m_height(height), m_position(position), m_direction(direction), m_up(up)
    {
        // ワールド座標系でのスクリーンの大きさ
        const Float screenWidth = screenSize * width / height;
        const Float screenHeight = screenSize;

        // スクリーンを張るベクトルを計算
        m_screenVectorX = Normalize(Cross(m_direction, m_up)) * screenWidth;
        m_screenVectorY = Normalize(Cross(m_screenVectorX, m_direction)) * screenHeight;

        // スクリーンの中心座標を計算
        m_screenCenter = m_position + m_direction * distanceToScreen;
    }

    // スクリーン上の特定のUV座標からレイを計算するメソッド
    Ray spawnRay(Float screenU, Float screenV) const
    {
        // UV座標を用いてスクリーン上のワールド座標に変換
        const Vec3 screenPosition =
            m_screenCenter + m_screenVectorX * screenU + m_screenVectorY * screenV;

        // カメラ位置からスクリーン上座標への方向ベクトルを計算
        const Vec3 direction = Normalize(screenPosition - m_position);

        // レイを生成して返す
        return Ray(m_position, direction);
    }

private:
    // 画像の解像度
    int m_width = 0;
    int m_height = 0;
    // カメラ位置
    Vec3 m_position = {};
    // カメラ姿勢
    Vec3 m_direction = {};
    Vec3 m_up = {};
    // スクリーンを張るベクトル
    Vec3 m_screenVectorX = {};
    Vec3 m_screenVectorY = {};
    // スクリーンの中心座標
    Vec3 m_screenCenter = {};
};