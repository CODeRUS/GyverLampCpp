#include "HorizontalRainbowEffect.h"

namespace  {

uint8_t hue = 0;

} // namespace

HorizontalRainbowEffect::HorizontalRainbowEffect()
{
    effectName = "Horizontal rainbow";
}

void HorizontalRainbowEffect::tick()
{
    hue += 2;
    for (uint8_t i = 0; i < width; i++) {
        const CHSV thisColor = CHSV((hue + i * settings->effectScale), 255, 255);
        for (uint8_t j = 0; j < height; j++) {
            myMatrix->drawPixelXY(i, j, thisColor);
        }
    }
}
