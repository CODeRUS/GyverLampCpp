#include "DoubleCometsEffect.h"

DoubleCometsEffect::DoubleCometsEffect()
{

}

void DoubleCometsEffect::tick()
{
    //myMatrix->dimAll(192);
    myMatrix->dimAll(255 - settings.scale * 2);

    // gelb im Kreis
    byte xx = 2 + sin8( millis() / 10) / 22;
    byte yy = 2 + cos8( millis() / 10) / 22;
    myMatrix->drawPixelXY(xx, yy, CRGB(0xFFFF00));

    // rot in einer Acht
    xx = 4 + sin8( millis() / 46) / 32;
    yy = 4 + cos8( millis() / 15) / 32;
    myMatrix->drawPixelXY(xx, yy, CRGB(0xFF0000));

    // Noise
    effectX[0] += 3000;
    effectY[0] += 3000;
    effectZ[0] += 3000;
    effectScaleX[0] = 8000;
    effectScaleY[0] = 8000;
    FillNoise(0);
    MoveFractionalNoiseX(3, 0.33f);
    MoveFractionalNoiseY(3);
}
