#include "EffectBall.h"

namespace {

int8_t ballSize = 1;
CRGB ballColor = CRGB::White;
float* vectorB = nullptr;
float* coordB = nullptr;

bool RANDOM_COLOR = true;

}

EffectBall::EffectBall(const String &id)
    : EffectCalc(id)
{

}

void EffectBall::activate()
{
    vectorB = new float[2]();
    coordB = new float[2]();

    byte _scale = map(settings.scale, 0, 100, 1, 255);

    if (_scale <= 85) {
        ballSize = map(_scale, 1, 85, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    } else if (_scale > 85 and _scale <= 170) {
        ballSize = map(_scale, 170, 86, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    } else {
        ballSize = map(_scale, 171, 255, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    }

    for (uint8_t i = 0U; i < 2U; i++) {
        coordB[i] = i? float(WIDTH - ballSize) / 2 : float(HEIGHT - ballSize) / 2;
        vectorB[i] = (float)random(0, 240) / 10.0f - 12.0f;
        ballColor = CHSV(random8(1, 255), 220, random8(100, 255));
    }
}

void EffectBall::deactivate()
{
    delete[] vectorB;
    delete[] coordB;
}

bool EffectBall::run()
{
    byte _speed = 255 - settings.speed;
    byte _scale = map(settings.scale, 0, 100, 1, 255);

    if (_scale <= 85) {
        ballSize = map(_scale, 1, 85, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    } else if (_scale > 85 and _scale <= 170) {
        ballSize = map(_scale, 170, 86, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    } else {
        ballSize = map(_scale, 171, 255, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    }

    // каждые 5 секунд коррекция направления
    EVERY_N_SECONDS(5) {
        //LOG(println,ballSize);
        for (uint8_t i = 0U; i < 2U; i++) {
            if (fabs(vectorB[i]) < 12) {
                vectorB[i] += (float)random8(0U, 80U) / 10.0f - 4.0f;
            } else if (vectorB[i] > 12) {
                vectorB[i] -= (float)random8(10, 60) / 10.0f;
            } else {
                vectorB[i] += (float)random8(10, 60) / 10.0f;
            }
            if (!(uint8_t)vectorB[i]) {
                vectorB[i] += vectorB[i] > 0 ? 0.25f : -0.25f;
            }

            // ballColor = CHSV(random8(1, 255), 220, random8(100, 255));
        }
    }

    for (uint8_t i = 0U; i < 2U; i++) {
        coordB[i] += vectorB[i] * ((0.1f * (float)_speed) /127.0f);
        if ((int8_t)coordB[i] < 0) {
            coordB[i] = 0;
            vectorB[i] = -vectorB[i];
            if (RANDOM_COLOR) {
                ballColor = CHSV(random8(1, 255), 220, random8(100, 255));
            }
        }
    }
    if ((int8_t)coordB[0U] > (int16_t)(WIDTH - ballSize)) {
        coordB[0U] = (WIDTH - ballSize);
        vectorB[0U] = -vectorB[0U];
        if (RANDOM_COLOR) {
            ballColor = CHSV(random8(1, 255), 220, random8(100, 255));
        }
    }
    if ((int8_t)coordB[1U] > (int16_t)(HEIGHT - ballSize)) {
        coordB[1U] = (HEIGHT - ballSize);
        vectorB[1U] = -vectorB[1U];
        if (RANDOM_COLOR) {
            ballColor = CHSV(random8(1, 255), 220, random8(100, 255));
        }
    }

    if (_scale <= 85) {
        FastLED.clear();
    } else if (_scale > 85 and _scale <= 170) {
        myMatrix->fadeToBlackBy(255 - (uint8_t)(10 * ((float)_speed) /255) + 30);
    } else {
        myMatrix->fadeToBlackBy(255 - (uint8_t)(10 * ((float)_speed) /255) + 15);
    }

    for (float i = 0.0f; i < (float)ballSize; i+= 0.25f) {
        for (float j = 0.0f; j < (float)ballSize; j+=0.25f) {
            EffectMath::drawPixelXYF(coordB[0U] + i, coordB[1U] + j, ballColor);
        }
    }
    return true;
}
