#include "PrismataEffect.h"

/*
* https://github.com/pixelmatix/aurora/blob/master/PatternPendulumWave.h
*
* Inspired by and based on a loading animation for Prismata by Lunarch Studios:
* http://www.reddit.com/r/gifs/comments/2on8si/connecting_to_server_so_mesmerizing/cmow0sz
*
* Lunarch Studios Inc. hereby publishes the Actionscript 3 source code pasted in this
* comment under the Creative Commons CC0 1.0 Universal Public Domain Dedication.
* Lunarch Studios Inc. waives all rights to the work worldwide under copyright law,
* including all related and neighboring rights, to the extent allowed by law.
* You can copy, modify, distribute and perform the work, even for commercial purposes,
* all without asking permission.
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

uint8_t hue = 0;

}

PrismataEffect::PrismataEffect(const String &id)
    : Effect(id)
{

}

void PrismataEffect::tick()
{
    EVERY_N_MILLIS(33) {
        hue++;
    }
    myMatrix->applyBlur2d(20);
    myMatrix->dimAll(255 - settings.scale % 11);

    for (int x = 0; x < mySettings->matrixSettings.width; x++) {
        uint8_t beat = (GET_MILLIS() * (accum88(x + 1)) * 28 * (255 - settings.speed)) >> 17;
        uint8_t y = scale8(sin8(beat), mySettings->matrixSettings.height - 1);
        myMatrix->drawPixelXY(x, y, ColorFromPalette(*myMatrix->GetColorPalette(settings.scale), x * 7 + hue));
    }
}
