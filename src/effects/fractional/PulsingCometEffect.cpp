#include "PulsingCometEffect.h"

PulsingCometEffect::PulsingCometEffect(const String &id)
    : FractionalEffect(id)
{

}

void PulsingCometEffect::tick()
{
    //myMatrix->dimAll(184);
    myMatrix->dimAll(255 - settings.scale * 2);

    CRGB _eNs_color = CHSV(millis(), 255, 255);
    myMatrix->drawPixelXY(myMatrix->getCenterX(), myMatrix->getCenterY(), _eNs_color);
    // Noise
    effectX[0] += 2000;
    effectY[0] += 2000;
    effectZ[0] += 2000;
    effectScaleX[0] = 4000;
    effectScaleY[0] = 4000;
    FillNoise(0);
    MoveFractionalNoiseX(6);
    MoveFractionalNoiseY(5, -0.5);
}
