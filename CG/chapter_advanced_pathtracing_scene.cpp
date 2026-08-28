#include "camera.h"
#include "random.h"
#include "real.h"
#include "scene_figure18.h"
#include "sphere.h"
#include "utility.h"

int main()
{
    const int IMAGE_WIDTH = 500;
    const int IMAGE_HEIGHT = 360;

    const int MAX_BOUNCE = 16;
    const int SAMPLE_PER_PIXEL = 4096;
    const Color ENVIRONMENT_COLOR(0.65, 0.65, 0.65);

    // 右斜め上から見下ろし、球の前後関係と床面の奥行きを見せる
    const Vec3 CAMERA_POSITION(16.5, 14.4, 26.4);
    const Vec3 CAMERA_DIRECTION = Normalize(Vec3(-15.0, -10.4, -24.0));
    const Vec3 CAMERA_UP(0, 1, 0);
    const Float SCREEN_SIZE = 1.2;
    const Float DISTANCE_TO_SCREEN = 4.4;

    Image image(IMAGE_WIDTH, IMAGE_HEIGHT);
    Camera camera(IMAGE_WIDTH, IMAGE_HEIGHT, CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_UP,
                  SCREEN_SIZE, DISTANCE_TO_SCREEN);

    int currentProgress = 0;

    ParallelFor(IMAGE_HEIGHT, [&](int y) {
        for (int x = 0; x < IMAGE_WIDTH; ++x)
        {
            const int pixelID = x + y * IMAGE_WIDTH;
            Random random(pixelID);
            Color accumulatedColor;

            for (int sampleIndex = 0; sampleIndex < SAMPLE_PER_PIXEL; ++sampleIndex)
            {
                const Float subpixelU = random.getFloatInRange();
                const Float subpixelV = random.getFloatInRange();
                const Float screenU = Float((x + subpixelU) * 2) / IMAGE_WIDTH - Float(1);
                const Float screenV = Float(1) - Float((y + subpixelV) * 2) / IMAGE_HEIGHT;

                Ray ray = camera.spawnRay(screenU, screenV);
                Color throughput(1, 1, 1);

                for (int bounceCount = 0; bounceCount < MAX_BOUNCE; ++bounceCount)
                {
                    if (!CheckSceneIntersectionAndUpdateRay(ray))
                    {
                        // 無限遠の白い環境光
                        accumulatedColor += throughput * ENVIRONMENT_COLOR;
                        break;
                    }

                    const auto &sphere = GetSceneSphere(ray.intersectedPrimitiveID);
                    const auto &material = GetSceneMaterial(sphere.materialID);
                    const auto intersectionPoint = ray.getIntersectionPoint();
                    accumulatedColor += throughput * material.getEmissionColor();

                    const Float randomValue0 = random.getFloatInRange();
                    const Float randomValue1 = random.getFloatInRange();
                    const Float diffuseRatio = material.getDiffuseRatio();

                    if (random.getFloatInRange() < diffuseRatio)
                    {
                        ray = material.sampleNextDiffuseDirection(
                            intersectionPoint, sphere.computeNormal(intersectionPoint), ray,
                            randomValue0, randomValue1);
                        throughput = throughput * material.getDiffuseColor();
                    }
                    else
                    {
                        ray = material.sampleNextMirrorDirection(
                            intersectionPoint, sphere.computeNormal(intersectionPoint), ray,
                            randomValue0, randomValue1);
                        throughput = throughput * material.getMirrorColor();
                    }
                }
            }

            image.setPixel(x, y, accumulatedColor / SAMPLE_PER_PIXEL);
        }
        UpdateProgressBar(currentProgress, IMAGE_HEIGHT);
    });

    SavePNG("chapter_advanced_pathtracing_scene.png", image);
    return 0;
}
