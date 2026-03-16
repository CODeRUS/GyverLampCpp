#pragma once
#include <FastLED.h>

class EffectMath
{
public:
    EffectMath();

    static void drawPixelXY(uint8_t x, uint8_t y, const CRGB &color);
    static void blur2d(uint8_t val);
    static void drawCircle(int x0, int y0, int radius, const CRGB &color);
    static void fader(uint8_t step);
    static uint32_t getPixColorXY(uint8_t x, uint8_t y);
    static void setLed(uint16_t idx, CRGB val);
    static float randomf(float min, float max);
    static void dimAll(uint8_t value);
    static void drawPixelXYF_Y(uint16_t x, float y, const CRGB &color, uint8_t darklevel = 50);
    static void drawPixelXYF(float x, float y, const CRGB &color, uint8_t darklevel = 25);
    static CRGB makeDarker(const CRGB& color, fract8 howMuchDarker = 5);

    template <typename T>
    static T fmap(const T x, const T in_min, const T in_max, const T out_min, const T out_max);
};

