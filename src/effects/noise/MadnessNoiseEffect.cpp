#include "MadnessNoiseEffect.h"

MadnessNoiseEffect::MadnessNoiseEffect()
{
    effectName = "Madness 3D";
}

void MadnessNoiseEffect::tick()
{
    fillNoise8();
    for (uint8_t i = 0; i < width; i++) {
        for (uint8_t j = 0; j < height; j++) {
            CRGB thisColor = CHSV(noise[j][i], 255, noise[i][j]);
            MyLED::drawPixelXY(i, j, thisColor);
        }
    }
}
