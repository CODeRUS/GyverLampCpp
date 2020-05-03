#include "RainbowCometEffect.h"

RainbowCometEffect::RainbowCometEffect()
{

}

void RainbowCometEffect::tick()
{
    myMatrix->dimAll(254); // < -- затухание эффекта для последующего кадра
    CRGB _eNs_color = CHSV(millis() / settings.scale * 2, 255, 255);
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
