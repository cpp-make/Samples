#include <string>
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#pragma warning(disable : 4996)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "real.h"
#include "utility.h"
#include "vec.h"

// ガンマ変換などを行い、HDR値をLDR値に変換する関数
uint8_t ConvertToLDRValue(Float x)
{
    const Float GAMMA_CORRECTION_FACTOR = Float(1 / 2.2);
    const Float clampedValue = std::clamp(x, Float(0), Float(1));
    return uint8_t(std::pow(clampedValue, GAMMA_CORRECTION_FACTOR) * 255 + Float(0.5));
}

// ConvertToLDRValue()を用いて、値の範囲制限のないHDR画像（image変数）を範囲制限（0-255）のあるLDR画像に変換する
// そして、変換後のLDR画像をPNGとして保存する
void SavePNG(const std::string &filename, const Image &image)
{
    // サイズが sizeof(uint8_t) * 3 * width * height のvectorを作る
    std::vector<std::array<uint8_t, 3>> ldrImage(image.getWidth() * image.getHeight());

    for (int y = 0; y < image.getHeight(); ++y)
    {
        for (int x = 0; x < image.getWidth(); ++x)
        {
            Color sourcePixel = image.getPixel(x, y);
            const int pixelIndex = x + y * image.getWidth();
            auto &destinationPixel = ldrImage[pixelIndex];
            destinationPixel[0] = ConvertToLDRValue(sourcePixel.x);
            destinationPixel[1] = ConvertToLDRValue(sourcePixel.y);
            destinationPixel[2] = ConvertToLDRValue(sourcePixel.z);
        }
    }

    // 変換されたLDR画像データをPNGファイルとして書き出す
    stbi_write_png(filename.c_str(), image.getWidth(), image.getHeight(), 3, ldrImage.data(),
                   3 * image.getWidth());
}
