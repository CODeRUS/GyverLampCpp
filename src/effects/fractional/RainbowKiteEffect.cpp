#include "RainbowKiteEffect.h"

RainbowKiteEffect::RainbowKiteEffect(const String &id)
    : FractionalEffect(id)
{

}

void RainbowKiteEffect::tick()
{
    myMatrix->dimAll(96);
    //myMatrix->dimAll(255 - settings.scale * 2);
    for (uint8_t y = 2; y < mySettings->matrixSettings.height; y += 5) {
        for (uint8_t x = 2; x < mySettings->matrixSettings.width; x += 5) {
            myMatrix->drawPixelXY(x, y, CHSV(x * y , 255, 255));
            myMatrix->drawPixelXY(x + 1, y, CHSV((x + 4) * y, 255, 255));
            myMatrix->drawPixelXY(x, y + 1, CHSV(x * (y + 4), 255, 255));
            myMatrix->drawPixelXY(x + 1, y + 1, CHSV((x + 4) * (y + 4), 255, 255));
        }
    }
    // Noise
    effectX[0] += 3000;
    effectY[0] += 3000;
    effectZ[0] += 3000;
    effectScaleX[0] = 8000;
    effectScaleY[0] = 8000;
    FillNoise(0);
    MoveFractionalNoiseX(3);
    MoveFractionalNoiseY(3);
}
