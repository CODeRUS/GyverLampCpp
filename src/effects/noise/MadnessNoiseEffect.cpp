#include "MadnessNoiseEffect.h"

MadnessNoiseEffect::MadnessNoiseEffect(const String &id)
    : NoiseEffect(id)
{
}

void MadnessNoiseEffect::tick()
{
    fillNoise8();
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        for (uint8_t j = 0; j < mySettings->matrixSettings.height; j++) {
            CRGB thisColor = CHSV(noise[j][i], 255, noise[i][j]);
            myMatrix->drawPixelXY(i, j, thisColor);
        }
    }
}
