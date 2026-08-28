#include <algorithm>

#include "camera.h"
#include "real.h"
#include "scene.h"
#include "sphere.h"
#include "utility.h"

int main()
{
    // レンダリング画像の解像度
    const int IMAGE_WIDTH = 500;
    const int IMAGE_HEIGHT = 360;

    // シーン上のカメラ情報
    const Vec3 CAMERA_POSITION(0, 5, 20);
    const Vec3 CAMERA_DIRECTION = Normalize(Vec3(0, 0, -1));
    const Vec3 CAMERA_UP(0, 1, 0);

    // シーン上のスクリーン情報（画像と対応）
    const Float SCREEN_SIZE = 3;        // スクリーンのサイズ
    const Float DISTANCE_TO_SCREEN = 4; // カメラからスクリーンまでの距離

    // シーン上の光源方向
    const Vec3 LIGHT_DIRECTION = Normalize(Vec3(1, 1, 0.3));

    Image image(IMAGE_WIDTH, IMAGE_HEIGHT);
    Camera camera(IMAGE_WIDTH, IMAGE_HEIGHT, CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_UP,
                  SCREEN_SIZE, DISTANCE_TO_SCREEN);

    for (int y = 0; y < IMAGE_HEIGHT; ++y)
    {
        for (int x = 0; x < IMAGE_WIDTH; ++x)
        {
            // ピクセル座標からスクリーン上のUV座標に変換
            const Float screenU = Float(x * 2) / IMAGE_WIDTH - Float(1);
            const Float screenV = Float(1) - Float(y * 2) / IMAGE_HEIGHT;

            // スクリーンUVからレイを計算、カメラから発射
            Ray ray = camera.spawnRay(screenU, screenV);
            Color color(1, 1, 1);

            // レイとシーンの交差判定を行い、レイを更新する
            if (CheckSceneIntersectionAndUpdateRay(ray))
            {
                // 交点を取得する
                const Vec3 intersectionPoint = ray.getIntersectionPoint();
                // 交点における法線を計算する
                const Vec3 normal =
                    GetSceneSphere(ray.intersectedPrimitiveID).computeNormal(intersectionPoint);

                // 交差したプリミティブのIDを直接参照し、表面色（アルベド）を決定してみる
                Color surfaceAlbedo;
                switch (ray.intersectedPrimitiveID)
                {
                case 0:
                    surfaceAlbedo = Color(0.2, 1, 0.2);
                    break;
                case 1:
                    surfaceAlbedo = Color(0.2, 0.2, 1);
                    break;
                case 2:
                    surfaceAlbedo = Color(1, 0.2, 0.2);
                    break;
                default:
                    surfaceAlbedo = Color(0.8, 0.8, 0.8);
                    break;
                }

                // 光源方向に再びレイトレーシングを行い、ピクセルが影に入っているかどうかを判定する
                Ray shadowRay(intersectionPoint, LIGHT_DIRECTION, ray.intersectedPrimitiveID);

                // 影に入っている場合は1、入っていない場合は0
                Float shadowed = 0;
                if (CheckSceneIntersectionAndUpdateRay(shadowRay))
                {
                    shadowed = 1;
                }

                // 上記の計算結果を用いて、ディフューズシェーディングを行う
                color = surfaceAlbedo *
                        std::clamp(Dot(normal, LIGHT_DIRECTION), Float(0), Float(1)) *
                        (Float(1) - shadowed);
            }
            image.setPixel(x, y, color);
        }
    }
    SavePNG("chapter_raytracing.png", image);
    return 0;
}
