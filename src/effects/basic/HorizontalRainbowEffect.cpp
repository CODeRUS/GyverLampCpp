#include "HorizontalRainbowEffect.h"

namespace  {

uint8_t hue = 0;

} // namespace

HorizontalRainbowEffect::HorizontalRainbowEffect()
{
}

void HorizontalRainbowEffect::tick()
{
    hue += 2;
    for (uint8_t i = 0; i < width; i++) {
        const CHSV thisColor = CHSV((hue + i * scale()), 255, 255);
        for (uint8_t j = 0; j < height; j++) {
            myMatrix->drawPixelXY(i, j, thisColor);
        }
    }
}
