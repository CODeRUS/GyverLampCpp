#include "PrismataEffect.h"

namespace {

uint8_t hue = 0;

}

PrismataEffect::PrismataEffect()
{

}

void PrismataEffect::tick()
{
    EVERY_N_MILLIS(33) {
        hue++;
    }
    myMatrix->applyBlur2d(20);
    myMatrix->dimAll(255 - settings.scale % 11);

    for (int x = 0; x < mySettings->matrixSettings.width; x++) {
        uint8_t beat = (GET_MILLIS() * (accum88(x + 1)) * 28 * settings.speed) >> 17;
        uint8_t y = scale8(sin8(beat), mySettings->matrixSettings.height - 1);
        myMatrix->drawPixelXY(x, y, ColorFromPalette(*myMatrix->GetPalette(settings.scale), x * 7 + hue));
    }
}
