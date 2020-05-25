#include "WaveEffect.h"

/*
 * https://github.com/pixelmatix/aurora/blob/master/PatternWave.h
 *
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

namespace {

uint8_t waveThetaUpdate = 0;
uint8_t waveThetaUpdateFrequency = 0;
uint8_t waveTheta = 0;

uint8_t hueUpdate = 0;
uint8_t hueUpdateFrequency = 0;
uint8_t hue = 0;

uint8_t waveRotation = 0;
uint8_t waveScale = 16;
uint8_t waveCount = 1;

}

WaveEffect::WaveEffect(const String &id)
    : Effect(id)
{

}

void WaveEffect::activate()
{
    waveScale = 256 / mySettings->matrixSettings.width;

    waveRotation = (settings.scale - 1) / 25;
    waveCount = settings.speed % 2;
}

void WaveEffect::tick()
{
    myMatrix->dimAll(254);

    int n = 0;

    switch (waveRotation) {
    case 0:
        for (int x = 0; x < mySettings->matrixSettings.width; x++) {
            n = quadwave8(x * 2 + waveTheta) / waveScale;
            myMatrix->drawPixelXY(x,
                                  n,
                                  ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                   hue + x));
            if (waveCount != 1)
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - n,
                                      ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                       hue + x));
        }
        break;

    case 1:
        for (int y = 0; y < mySettings->matrixSettings.height; y++) {
            n = quadwave8(y * 2 + waveTheta) / waveScale;
            myMatrix->drawPixelXY(n,
                                  y,
                                  ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                   hue + y));
            if (waveCount != 1)
                myMatrix->drawPixelXY(mySettings->matrixSettings.width - 1 - n,
                                      y,
                                      ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                       hue + y));
        }
        break;

    case 2:
        for (int x = 0; x < mySettings->matrixSettings.width; x++) {
            n = quadwave8(x * 2 - waveTheta) / waveScale;
            myMatrix->drawPixelXY(x,
                                  n,
                                  ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                   hue + x));
            if (waveCount != 1)
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - n,
                                      ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                       hue + x));
        }
        break;

    case 3:
        for (int y = 0; y < mySettings->matrixSettings.height; y++) {
            n = quadwave8(y * 2 - waveTheta) / waveScale;
            myMatrix->drawPixelXY(n,
                                  y,
                                  ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                   hue + y));
            if (waveCount != 1)
                myMatrix->drawPixelXY(mySettings->matrixSettings.width - 1 - n,
                                      y,
                                      ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                       hue + y));
        }
        break;
    }

    if (waveThetaUpdate >= waveThetaUpdateFrequency) {
        waveThetaUpdate = 0;
        waveTheta++;
    }
    else {
        waveThetaUpdate++;
    }

    if (hueUpdate >= hueUpdateFrequency) {
        hueUpdate = 0;
        hue++;
    }
    else {
        hueUpdate++;
    }

    myMatrix->applyBlur2d(20);
}
