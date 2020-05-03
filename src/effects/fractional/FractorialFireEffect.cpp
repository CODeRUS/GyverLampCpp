#include "FractorialFireEffect.h"

FractorialFireEffect::FractorialFireEffect()
{

}

void FractorialFireEffect::tick()
{
    myMatrix->applyBlur2d(20);
    //myMatrix->dimAll(140);
    myMatrix->dimAll(255 - settings.scale * 2);
    for (uint8_t i = 1; i < mySettings->matrixSettings.width; i += 2) {
        myMatrix->drawPixelXY(i, mySettings->matrixSettings.width - 1, CHSV(i * 2, 255, 255));
    }
    // Noise
    e_x[0] += 3000;
    e_y[0] += 3000;
    e_z[0] += 3000;
    e_scaleX[0] = 8000;
    e_scaleY[0] = 8000;
    FillNoise(0);
    //MoveX(1);
    //MoveY(1);
    MoveFractionalNoiseY(2, 1);
    MoveFractionalNoiseX(2);
}
