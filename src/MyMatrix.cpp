#include "MyMatrix.h"

// #define FASTLED_INTERRUPT_RETRY_COUNT 0
// #define FASTLED_ALLOW_INTERRUPTS 0
#if defined(ESP8266)
#define FASTLED_ESP8266_RAW_PIN_ORDER
#endif

namespace  {

#if defined(SONOFF)
const uint8_t ledPin = 14;
#else
const uint8_t ledPin = 13;
#endif

uint16_t numLeds = 0;

CRGB* leds = nullptr;

const uint32_t defaultMaxCurrent = 1000;

MyMatrix *instance = nullptr;

} // namespace

MyMatrix *MyMatrix::Instance()
{
    return instance;
}

void MyMatrix::Initialize(uint8_t sizeWidth, uint8_t sizeHeight, uint8_t matrixType)
{
    if (instance) {
        return;
    }

    numLeds = sizeWidth * sizeHeight;
    leds = new CRGB[numLeds]();
    FastLED.addLeds<WS2812B, ledPin, GRB>(leds, numLeds);

    instance = new MyMatrix(leds, sizeWidth, sizeHeight, matrixType);

    instance->begin();
    instance->clear();
    instance->show();
}

void MyMatrix::setCurrentLimit(uint32_t maxCurrent)
{
    FastLED.setMaxPowerInVoltsAndMilliamps(5, maxCurrent);
}

MyMatrix::MyMatrix(CRGB *leds, uint8_t w, uint8_t h, uint8_t matrixType)
    : FastLED_NeoMatrix(leds, w, h, 1, 1, matrixType)
{
}

void MyMatrix::fill(CRGB color, bool shouldShow)
{
    fillScreen(color);
    if (shouldShow) {
        show();
    }
}

void MyMatrix::fillProgress(double progress, CRGB color)
{
    const uint16_t number = static_cast<uint16_t>(numLeds * progress);
    fill_solid(leds, number, color);
    show();
}

void MyMatrix::setLed(uint8_t x, uint8_t y, CRGB color, bool showuldShow)
{
    drawPixel(x, y, color);
    if (showuldShow) {
        show();
    }
}

void MyMatrix::setLed(uint16_t index, CRGB color, bool showuldShow)
{
    leds[index] = color;
    if (showuldShow) {
        show();
    }
}

void MyMatrix::fadeToBlackBy(uint16_t index, uint8_t step)
{
    leds[index].fadeToBlackBy(step);
}

void MyMatrix::matrixTest()
{
    for (int16_t xx = 0; xx < width(); xx++) {
        clear();
        drawPixel(xx, 0, CRGB(CRGB::Red));
        show();

        delay(200);
    }

    for (int16_t yy = 0; yy < height(); yy++) {
        clear();
        drawPixel(0, yy, CRGB(CRGB::Green));
        show();

        delay(200);
    }

    for (int16_t i = 0; i < 16; i++) {
        clear();
        drawPixel(i, i, Color(255, 255, 255));
        show();
        delay(200);
    }

    clear();
    show();
}

uint16_t MyMatrix::getPixelNumber(uint8_t x, uint8_t y)
{
    return static_cast<uint16_t>(XY(x, y));
}

void MyMatrix::drawPixelXY(uint8_t x, uint8_t y, CRGB color)
{
    drawPixel(x, y, color);
}

CRGB MyMatrix::getPixColor(uint16_t number)
{
    if (number > numLeds - 1) {
        return 0;
    }
    return leds[number];
}

CRGB MyMatrix::getPixColorXY(uint8_t x, uint8_t y)
{
    return getPixColor(getPixelNumber(x, y));
}
