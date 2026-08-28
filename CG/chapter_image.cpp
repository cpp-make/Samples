#include "real.h"
#include "utility.h"

int main()
{
    // レンダリング画像の解像度
    const int IMAGE_WIDTH = 512;
    const int IMAGE_HEIGHT = 512;
    Image image(IMAGE_WIDTH, IMAGE_HEIGHT);

    for (int y = 0; y < IMAGE_HEIGHT; ++y)
    {
        for (int x = 0; x < IMAGE_WIDTH; ++x)
        {
            // ピクセルの位置に基づいて色を計算
            // ここではグラデーションを生成している
            const Color pixelColor(Float(x) / IMAGE_WIDTH, Float(y) / IMAGE_HEIGHT, Float(0));
            image.setPixel(x, y, pixelColor);
        }
    }
    SavePNG("chapter_image.png", image);
    return 0;
}