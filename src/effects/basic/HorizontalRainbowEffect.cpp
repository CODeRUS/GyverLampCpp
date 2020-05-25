#include "HorizontalRainbowEffect.h"

namespace  {

uint8_t hue = 0;

} // namespace

HorizontalRainbowEffect::HorizontalRainbowEffect(const String &id)
    : Effect(id)
{
}

void HorizontalRainbowEffect::tick()
{
    hue += 2;
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        const CHSV thisColor = CHSV((hue + i * settings.scale), 255, 255);
        for (uint8_t j = 0; j < mySettings->matrixSettings.height; j++) {
            myMatrix->drawPixelXY(i, j, thisColor);
        }
    }
}
