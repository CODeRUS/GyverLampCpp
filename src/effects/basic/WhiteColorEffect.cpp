#include "WhiteColorEffect.h"

WhiteColorEffect::WhiteColorEffect(const String &id)
    : Effect(id)
{

}

void WhiteColorEffect::tick()
{
    uint8_t centerY = max((float)round(mySettings->matrixSettings.width / 2.0f) - 1.0f, 0.0f);
    uint8_t bottomOffset = (uint8_t)(!(mySettings->matrixSettings.height & 1) && (mySettings->matrixSettings.height > 1));
    for (int16_t y = centerY; y >= 0; y--) {
        CRGB color = CHSV(
                    45,
                    map(settings.speed, 0, 255, 0, 170),
                    y == centerY
                    ? 255
                    : (settings.scale / 100.0f) > ((centerY + 1.0f) - (y + 1.0f)) / (centerY + 1.0f) ? 255 : 0);

        for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
            myMatrix->drawPixelXY(x, y, color);
            myMatrix->drawPixelXY(x, max((uint8_t)(mySettings->matrixSettings.height - 1) - (y + 1) + bottomOffset, 0), color);
        }
    }
}
