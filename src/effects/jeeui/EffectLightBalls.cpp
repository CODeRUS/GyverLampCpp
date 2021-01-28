#include "EffectLightBalls.h"

namespace {

uint8_t BORDERTHICKNESS = 1;

}

EffectLightBalls::EffectLightBalls(const String &id)
    : EffectCalc(id)
{

}

bool EffectLightBalls::run()
{
    const uint8_t paintWidth = WIDTH - BORDERTHICKNESS * 2;
    const uint8_t paintHeight = HEIGHT - BORDERTHICKNESS * 2;

    // Apply some blurring to whatever's already on the matrix
    // Note that we never actually clear the matrix, we just constantly
    // blur it repeatedly.  Since the blurring is 'lossy', there's
    // an automatic trend toward black -- by design.
    uint8_t blurAmount = dim8_raw(beatsin8(3,64,100));
    myMatrix->applyBlur2d(blurAmount);

    float speedScale = (((float)settings.speed)/255.0)+0.1;

    // Use two out-of-sync sine waves
    uint16_t  i = beatsin16( 79*speedScale, 0, 255); //91
    uint16_t  j = beatsin16( 67*speedScale, 0, 255); //109
    uint16_t  k = beatsin16( 53*speedScale, 0, 255); //73
    uint16_t  m = beatsin16( 97*speedScale, 0, 255); //123

    // The color of each point shifts over time, each at a different speed.
    uint16_t ms = millis() / (settings.scale/16 + 1);
    myMatrix->adjustPixelXY(highByte(i * paintWidth) + BORDERTHICKNESS,
                            highByte(j * paintHeight) + BORDERTHICKNESS,
                            CHSV( ms / 29, 200U, 255U));
    myMatrix->adjustPixelXY(highByte(j * paintWidth) + BORDERTHICKNESS,
                            highByte(k * paintHeight) + BORDERTHICKNESS,
                            CHSV( ms / 41, 200U, 255U));
    myMatrix->adjustPixelXY(highByte(k * paintWidth) + BORDERTHICKNESS,
                            highByte(m * paintHeight) + BORDERTHICKNESS,
                            CHSV( ms / 37, 200U, 255U));
    myMatrix->adjustPixelXY(highByte(m * paintWidth) + BORDERTHICKNESS,
                            highByte(i * paintHeight) + BORDERTHICKNESS,
                            CHSV( ms / 53, 200U, 255U));

    return true;
}
