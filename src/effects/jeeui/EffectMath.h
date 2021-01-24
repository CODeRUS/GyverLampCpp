#pragma once
#include <FastLED.h>

class EffectMath
{
public:
    EffectMath();

    static void drawPixelXY(int16_t x, int16_t y, const CRGB &color);
    static void blur2d(uint8_t val);
    static void drawCircle(int x0, int y0, int radius, const CRGB &color);
    static void fader(uint8_t step);
    static uint32_t getPixColorXY(uint16_t x, uint16_t y);
    static void setLed(uint16_t idx, CRGB val);
};

