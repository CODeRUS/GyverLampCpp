#include "VerticalRainbowEffect.h"

namespace  {

uint8_t hue = 0;

} // namespace

VerticalRainbowEffect::VerticalRainbowEffect(const String &id)
    : Effect(id)
{
}

void VerticalRainbowEffect::tick()
{
    hue += 2;
    for (uint8_t j = 0; j < mySettings->matrixSettings.height; j++) {
        const CHSV thisColor = CHSV((hue + j * settings.scale), 255, 255);
        for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
            myMatrix->drawPixelXY(i, j, thisColor);
        }
    }
}
