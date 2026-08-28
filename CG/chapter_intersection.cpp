#include "camera.h"
#include "real.h"
#include "sphere.h"
#include "utility.h"

int main()
{
    // レンダリング画像の解像度
    const int IMAGE_WIDTH = 500;
    const int IMAGE_HEIGHT = 360;

    // シーン上のカメラ情報
    const Vec3 CAMERA_POSITION(2, 2, 2);
    const Vec3 CAMERA_DIRECTION = Normalize(Vec3(-1, -1, -1));
    const Vec3 CAMERA_UP(0, 1, 0);

    // シーン上のスクリーン情報（画像と対応）
    const Float SCREEN_SIZE = 3;        // スクリーンのサイズ
    const Float DISTANCE_TO_SCREEN = 4; // カメラからスクリーンまでの距離

    Image image(IMAGE_WIDTH, IMAGE_HEIGHT);
    Camera camera(IMAGE_WIDTH, IMAGE_HEIGHT, CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_UP,
                  SCREEN_SIZE, DISTANCE_TO_SCREEN);

    // シーンには球体を1個だけ配置する
    Sphere sphere(1.5, Vec3(0, 0, 0), 0);

    for (int y = 0; y < IMAGE_HEIGHT; ++y)
    {
        for (int x = 0; x < IMAGE_WIDTH; ++x)
        {
            // ピクセル座標からスクリーン上のUV座標に変換
            const Float screenU = Float(x * 2) / IMAGE_WIDTH - Float(1);
            const Float screenV = Float(1) - Float(y * 2) / IMAGE_HEIGHT;

            // スクリーンUVからレイを計算、カメラから発射
            Ray ray = camera.spawnRay(screenU, screenV);

            // レイと球の交差判定を行い、交差したら赤(1, 0, 0)を出力する
            // 交差しなかったら白(1, 1, 1)を出力する
            if (sphere.checkIntersectionAndUpdateRay(ray))
            {
                image.setPixel(x, y, Color(1, 0, 0));
            }
            else
            {
                image.setPixel(x, y, Color(1, 1, 1));
            }
        }
    }
    SavePNG("chapter_intersection.png", image);
    return 0;
}
