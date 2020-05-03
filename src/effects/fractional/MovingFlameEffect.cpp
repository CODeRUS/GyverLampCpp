#include "MovingFlameEffect.h"

MovingFlameEffect::MovingFlameEffect()
{

}

void MovingFlameEffect::tick()
{
    myMatrix->applyBlur2d(20);
    //myMatrix->dimAll(160);
    myMatrix->dimAll(255 - settings.scale * 2);
    for (uint8_t i = 1; i < mySettings->matrixSettings.width; i += 3) {
        myMatrix->drawPixelXY(i, myMatrix->GetCenterY(), CHSV(i * 2 , 255, 255));
    }
    // Noise
    effectX[0] += 3000;
    effectY[0] += 3000;
    effectZ[0] += 3000;
    effectScaleX[0] = 8000;
    effectScaleY[0] = 8000;
    FillNoise(0);
    MoveFractionalNoiseY(3);
    MoveFractionalNoiseX(3);
}
