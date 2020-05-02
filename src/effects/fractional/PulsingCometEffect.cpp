#include "PulsingCometEffect.h"

PulsingCometEffect::PulsingCometEffect()
{

}

void PulsingCometEffect::tick()
{
    //myMatrix->dimAll(184);
    myMatrix->dimAll(255 - settings.scale * 2);

    CRGB _eNs_color = CHSV(millis(), 255, 255);
    myMatrix->drawPixelXY(myMatrix->GetCenterX(), myMatrix->GetCenterY(), _eNs_color);
    // Noise
    e_x[0] += 2000;
    e_y[0] += 2000;
    e_z[0] += 2000;
    e_scaleX[0] = 4000;
    e_scaleY[0] = 4000;
    FillNoise(0);
    MoveFractionalNoiseX(6);
    MoveFractionalNoiseY(5, -0.5);
}
