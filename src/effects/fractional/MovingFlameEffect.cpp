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
    e_x[0] += 3000;
    e_y[0] += 3000;
    e_z[0] += 3000;
    e_scaleX[0] = 8000;
    e_scaleY[0] = 8000;
    FillNoise(0);
    MoveFractionalNoiseY(3);
    MoveFractionalNoiseX(3);
}
