#include "ColorCometEffect.h"

ColorCometEffect::ColorCometEffect()
{

}

void ColorCometEffect::tick()
{
    myMatrix->dimAll(254);
    CRGB _eNs_color = CRGB::White;
    if (settings.scale < 100) {
        _eNs_color = CHSV(settings.scale * 2.57, 255, 255);
    }
    myMatrix->drawPixelXY(myMatrix->GetCenterX(), myMatrix->GetCenterY(), _eNs_color);
    myMatrix->drawPixelXY(myMatrix->GetCenterX() + 1, myMatrix->GetCenterY(), _eNs_color);
    myMatrix->drawPixelXY(myMatrix->GetCenterX(), myMatrix->GetCenterY() + 1, _eNs_color);
    myMatrix->drawPixelXY(myMatrix->GetCenterX() + 1, myMatrix->GetCenterY() + 1, _eNs_color);

    // Noise
    effectX[0] += 1500;
    effectY[0] += 1500;
    effectZ[0] += 1500;
    effectScaleX[0] = 8000;
    effectScaleY[0] = 8000;
    FillNoise(0);
    MoveFractionalNoiseX(mySettings->matrixSettings.width / 2 - 1);
    MoveFractionalNoiseY(mySettings->matrixSettings.height / 2 - 1);
}
