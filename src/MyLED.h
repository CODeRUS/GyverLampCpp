#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>

class MyLED
{
public:
    static void Initialize(uint8_t sizeWidth, uint8_t sizeHeight,
                           uint8_t matrixType = NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                                NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
    static void setCurrentLimit(uint32_t maxCurrent);
    static void setBrightness(uint8_t brightness);

    static uint8_t getWidth();
    static uint8_t getHeight();

    static void matrixTest();

    static void show();
    static void clear();
    static void fill(CRGB color, bool show = false);
    static void fillProgress(double progress, CRGB color);
    static void setLed(uint8_t x, uint8_t y, CRGB color, bool show = false);
    static void setLed(uint16_t index, CRGB color, bool show = false);
    static void fadeToBlackBy(uint16_t index, uint8_t step);

    static uint16_t getPixelNumber(uint8_t x, uint8_t y);
    static void drawPixelXY(uint8_t x, uint8_t y, CRGB color);
    static CRGB getPixColor(uint16_t number);
    static CRGB getPixColorXY(uint8_t x, uint8_t y);
};

