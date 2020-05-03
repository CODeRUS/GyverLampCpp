#include "FractionalEffect.h"

FractionalEffect::FractionalEffect(uint8_t layers)
    : numLayers(layers)
{

}

void FractionalEffect::activate()
{
    effectX = new uint32_t[numLayers];
    effectY = new uint32_t[numLayers];
    effectZ = new uint32_t[numLayers];
    effectScaleX = new uint32_t[numLayers];
    effectScaleY = new uint32_t[numLayers];

    noise3d = new uint8_t**[numLayers]();
    for (uint8_t i = 0; i < numLayers; ++i) {
        noise3d[i] = new uint8_t*[mySettings->matrixSettings.width];
        for (uint8_t j = 0; j < mySettings->matrixSettings.width; ++j) {
            noise3d[i][j] = new uint8_t[mySettings->matrixSettings.height];
        }
    }

    ledsbuff = new CRGB[myMatrix->GetNumLeds()];
}

void FractionalEffect::deactivate()
{
    delete[] effectX;
    delete[] effectY;
    delete[] effectZ;
    delete[] effectScaleX;
    delete[] effectScaleY;

    for (uint8_t i = 0; i < numLayers; ++i) {
        for (uint8_t j = 0; j < mySettings->matrixSettings.width; j++) {
            delete[] noise3d[i][j];
        }
        delete[] noise3d[i];
    }
    delete[] noise3d;

    delete[] ledsbuff;
}

void FractionalEffect::FillNoise(int8_t layer)
{
    for (int8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        int32_t ioffset = effectScaleX[layer] * (i - myMatrix->GetCenterX());
        for (int8_t j = 0; j < mySettings->matrixSettings.height; j++) {
            int32_t joffset = effectScaleY[layer] * (j - myMatrix->GetCenterY());
            int8_t data = inoise16(effectX[layer] + ioffset,
                                   effectY[layer] + joffset,
                                   effectZ[layer]) >> 8;
            int8_t olddata = noise3d[layer][i][j];
            int8_t newdata = scale8(olddata, 0) + scale8(data, 255);
            data = newdata;
            noise3d[layer][i][j] = data;
        }
    }
}

void FractionalEffect::MoveX(int8_t delta)
{
    for (uint8_t y = 0; y < mySettings->matrixSettings.height; y++) {
        for (uint8_t x = 0; x < mySettings->matrixSettings.width - delta; x++) {
            ledsbuff[myMatrix->XY(x, y)] = myMatrix->GetLeds()[myMatrix->XY(x + delta, y)];
        }
        for (uint8_t x = mySettings->matrixSettings.width - delta; x < mySettings->matrixSettings.width; x++) {
            ledsbuff[myMatrix->XY(x, y)] = myMatrix->GetLeds()[myMatrix->XY(x + delta - mySettings->matrixSettings.width, y)];
        }
    }
    memcpy(myMatrix->GetLeds(), ledsbuff, sizeof(CRGB) * myMatrix->GetNumLeds());
}

void FractionalEffect::MoveY(int8_t delta)
{
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        for (uint8_t y = 0; y < mySettings->matrixSettings.height - delta; y++) {
            ledsbuff[myMatrix->XY(x, y)] = myMatrix->GetLeds()[myMatrix->XY(x, y + delta)];
        }
        for (uint8_t y = mySettings->matrixSettings.height - delta; y < mySettings->matrixSettings.height; y++) {
            ledsbuff[myMatrix->XY(x, y)] = myMatrix->GetLeds()[myMatrix->XY(x, y + delta - mySettings->matrixSettings.height)];
        }
    }
    memcpy(myMatrix->GetLeds(), ledsbuff, sizeof(CRGB) * myMatrix->GetNumLeds());
}

void FractionalEffect::MoveFractionalNoiseX(int8_t amplitude, float shift)
{
    for (int8_t y = 0; y < mySettings->matrixSettings.height; y++) {
        int16_t amount = ((int16_t)noise3d[0][0][y] - 128) * 2 * amplitude + shift * 256  ;
        int8_t delta = amount >> 8 ;
        int8_t fraction = amount & 255;
        for (int8_t x = 0 ; x < mySettings->matrixSettings.width; x++) {
            if (amount < 0) {
                zD = x - delta; zF = zD - 1;
            } else {
                zD = x + delta; zF = zD + 1;
            }
            CRGB PixelA = CRGB::Black  ;
            if ((zD >= 0) && (zD < mySettings->matrixSettings.width)) {
                PixelA = myMatrix->GetLeds()[myMatrix->XY(zD, y)];
            }
            CRGB PixelB = CRGB::Black ;
            if ((zF >= 0) && (zF < mySettings->matrixSettings.width)) {
                PixelB = myMatrix->GetLeds()[myMatrix->XY(zF, y)];
            }
            ledsbuff[myMatrix->XY(x, y)] = (PixelA.nscale8(ease8InOutApprox(255 - fraction)))
                                         + (PixelB.nscale8(ease8InOutApprox(fraction)));   // lerp8by8(PixelA, PixelB, fraction );
        }
    }
    memcpy(myMatrix->GetLeds(), ledsbuff, sizeof(CRGB) * myMatrix->GetNumLeds());
}

void FractionalEffect::MoveFractionalNoiseY(int8_t amplitude, float shift)
{
    for (int8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        int16_t amount = ((int16_t)noise3d[0][x][0] - 128) * 2 * amplitude + shift * 256 ;
        int8_t delta = amount >> 8 ;
        int8_t fraction = amount & 255;
        for (int8_t y = 0 ; y < mySettings->matrixSettings.width; y++) {
            if (amount < 0) {
                zD = y - delta; zF = zD - 1;
            } else {
                zD = y + delta; zF = zD + 1;
            }
            CRGB PixelA = CRGB::Black ;
            if ((zD >= 0) && (zD < mySettings->matrixSettings.width)) {
                PixelA = myMatrix->GetLeds()[myMatrix->XY(x, zD)];
            }
            CRGB PixelB = CRGB::Black ;
            if ((zF >= 0) && (zF < mySettings->matrixSettings.width)) {
                PixelB = myMatrix->GetLeds()[myMatrix->XY(x, zF)];
            }
            ledsbuff[myMatrix->XY(x, y)] = (PixelA.nscale8(ease8InOutApprox(255 - fraction)))
                               + (PixelB.nscale8(ease8InOutApprox(fraction)));
        }
    }
    memcpy(myMatrix->GetLeds(), ledsbuff, sizeof(CRGB) * myMatrix->GetNumLeds());
}
