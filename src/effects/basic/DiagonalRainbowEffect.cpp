#include "DiagonalRainbowEffect.h"

namespace  {

uint8_t hue = 0;

} // namespace

DiagonalRainbowEffect::DiagonalRainbowEffect()
{
}

void DiagonalRainbowEffect::tick()
{
    hue += 2;
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        const CHSV thisColor = CHSV((hue + x * settings.scale), 255, 255);
        for (uint8_t y = 0; y < mySettings->matrixSettings.height; y++) {
            uint8_t pos = x + y;
            myMatrix->drawPixelXY(pos > 15 ? pos - 16 : pos, y, thisColor);
        }
    }
}
