#pragma once

#if defined(ESP8266)
#define FASTLED_ESP8266_RAW_PIN_ORDER
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ALLOW_INTERRUPTS 0
#endif

#define FASTLED_INTERNAL

#include <Arduino.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

#define myMatrix MyMatrix::instance()

class MyMatrix : public FastLED_NeoMatrix
{
public:
    static MyMatrix *instance();
    static void Initialize();

    uint8_t getRotation();

    const TProgmemRGBPalette16 *GetColorPalette(uint8_t pct);
    const CRGBPalette16 *GetFirePalette(uint8_t pct);

    uint16_t getNumLeds();
    uint8_t getCenterX();
    uint8_t getCenterY();
    uint8_t getDimension();

    CRGB *getLeds();

    void setCurrentLimit(uint32_t maxCurrent);

    void matrixTest();

    void clear(bool shouldShow = false);
    void fill(CRGB color, bool shouldShow = false);
    void fillProgress(double progress);
    void setLed(uint16_t index, CRGB color);
    void fadeToBlackBy(uint16_t index, uint8_t step);
    void fadeToBlackBy(uint8_t step);

    void dimAll(uint8_t value);
    uint8_t wrapX(int8_t x);
    uint8_t wrapY(int8_t y);
    void fader(uint8_t step);
    void fadePixel(uint8_t x, uint8_t y, uint8_t step);
    void fadePixel(uint16_t i, uint8_t step);
    uint32_t colorcode(const CRGB &color);
    void applyBlur2d(uint8_t amount);

    uint16_t getPixelNumberXY(uint8_t x, uint8_t y);

    void drawPixelXY(uint8_t x, uint8_t y, CRGB color);
    void drawLineXY(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, CRGB color);

    void adjustPixelXY(uint8_t x, uint8_t y, CRGB color);

    CRGB getPixColor(uint16_t number);
    CRGB getPixColorXY(uint8_t x, uint8_t y);

    void tintPixelXY(uint8_t x, uint8_t y, CRGB color);
    void shadePixelXY(uint8_t x, uint8_t y, CRGB color);
    void blendPixelXY(uint8_t x, uint8_t y, const CRGB &color, uint8_t amount);
    void dimPixelXY(uint8_t x, uint8_t y, uint8_t value);

    void fillRectXY(uint8_t x, uint8_t y, uint8_t w, uint8_t h, CRGB color);

    void fadePixelXY(uint8_t x, uint8_t y, uint8_t step);

    void getCharBounds(char c, int16_t *xx, int16_t *yy, uint16_t *ww, uint16_t *hh);

protected:
    MyMatrix(CRGB *leds, uint8_t w, uint8_t h, uint8_t matrixType);
};
