#include "RainbowKiteEffect.h"

RainbowKiteEffect::RainbowKiteEffect()
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
    e_x[0] += 3000;
    e_y[0] += 3000;
    e_z[0] += 3000;
    e_scaleX[0] = 8000;
    e_scaleY[0] = 8000;
    FillNoise(0);
    MoveFractionalNoiseX(3);
    MoveFractionalNoiseY(3);
}