#include <algorithm>

#include "camera.h"
#include "random.h"
#include "real.h"
#include "scene.h"
#include "sphere.h"
#include "utility.h"

int main()
{
    // レンダリング画像の解像度
    const int IMAGE_WIDTH = 500;
    const int IMAGE_HEIGHT = 360;

    // レイの反射回数の最大値とピクセル当たりのサンプリング数
    constexpr int MAX_BOUNCE = 16;
    constexpr int SAMPLE_PER_PIXEL = 32;

    // シーン上のカメラ情報
    const Vec3 CAMERA_POSITION(0, 5, 20);
    const Vec3 CAMERA_DIRECTION = Normalize(Vec3(0, 0, -1));
    const Vec3 CAMERA_UP(0, 1, 0);

    // シーン上のスクリーン情報（画像と対応）
    const Float SCREEN_SIZE = 3;        // スクリーンのサイズ
    const Float DISTANCE_TO_SCREEN = 4; // カメラからスクリーンまでの距離

    Image image(IMAGE_WIDTH, IMAGE_HEIGHT);
    Camera camera(IMAGE_WIDTH, IMAGE_HEIGHT, CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_UP,
                  SCREEN_SIZE, DISTANCE_TO_SCREEN);

    for (int y = 0; y < IMAGE_HEIGHT; ++y)
    {
        for (int x = 0; x < IMAGE_WIDTH; ++x)
        {
            // 個々のピクセルごとに、異なるpixelIDを計算し、乱数のシード値として使用する
            // これにより、ピクセルごとに異なる乱数を得ることができる
            const int pixelID = x + y * IMAGE_WIDTH;
            Random random(pixelID);

            // ピクセル座標からスクリーン上のUV座標に変換
            const Float screenU = Float(x * 2) / IMAGE_WIDTH - Float(1);
            const Float screenV = Float(1) - Float(y * 2) / IMAGE_HEIGHT;

            // 色の累積を保持する変数
            Color accumulatedColor;
            for (int sampleIndex = 0; sampleIndex < SAMPLE_PER_PIXEL; ++sampleIndex)
            {
                // スクリーンUVからレイを計算、カメラから発射
                Ray ray = camera.spawnRay(screenU, screenV);
                Color throughput(1, 1, 1);
                for (int bounceCount = 0; bounceCount < MAX_BOUNCE; ++bounceCount)
                {
                    // レイがオブジェクトとヒットしなかった場合、終了
                    if (!CheckSceneIntersectionAndUpdateRay(ray))
                    {
                        break;
                    }

                    // ヒットしたオブジェクト情報
                    const auto &sphere = GetSceneSphere(ray.intersectedPrimitiveID);
                    const auto &material = GetSceneMaterial(sphere.materialID);
                    auto intersectionPoint = ray.getIntersectionPoint();

                    // ヒットしたオブジェクトの発光成分を足しておく
                    accumulatedColor += throughput * material.getEmissionColor();

                    // 乱数を準備しておく
                    const Float randomValue0 = random.getFloatInRange();
                    const Float randomValue1 = random.getFloatInRange();

                    // ディフューズマテリアルにおける、次の反射方向をサンプリングする
                    ray = material.sampleNextDiffuseDirection(
                        intersectionPoint, sphere.computeNormal(intersectionPoint), ray,
                        randomValue0, randomValue1);
                    throughput = throughput * material.getDiffuseColor();
                }
            }
            // 最終的な色の計算
            const Color finalColor = accumulatedColor / SAMPLE_PER_PIXEL;
            image.setPixel(x, y, finalColor);
        }
    }
    SavePNG("chapter_pathtracing.png", image);
    return 0;
}
