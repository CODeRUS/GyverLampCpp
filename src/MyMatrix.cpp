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
    fill_solid(leds, numLeds, color);
    if (shouldShow) {
        show();
    }
}

void MyMatrix::fillProgress(double progress, CRGB color)
{
    clear();

    const uint16_t number = static_cast<uint16_t>(numLeds * progress);
    const String percent = String(static_cast<int>(progress * 100));

    const uint8_t fullRows = static_cast<uint8_t>(number / width());
    const uint8_t remainingProgress = static_cast<uint8_t>(number % width());

    if (fullRows > 0) {
        fillRectXY(0, 0, width() - 1, fullRows, Color(10, 10, 10));
        delay(1);
    }

    if (remainingProgress > 0) {
        drawLineXY(0, fullRows, remainingProgress, fullRows, Color(10, 10, 10));
        delay(1);
    }

    setCursor(0, 0);
    print(percent);
    delay(1);

    show();
}

void MyMatrix::setLed(uint8_t x, uint8_t y, CRGB color, bool showuldShow)
{
    drawPixelXY(x, y, color);
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
    for (int16_t i = 0; i < 16; i++) {
        clear();
        drawPixelXY(i, 0, CRGB(CRGB::Red));
        drawPixelXY(0, i, CRGB(CRGB::Green));
        drawPixelXY(i, i, CRGB(CRGB::White));
        show();
        delay(100);
    }

    clear();
    show();
}

uint16_t MyMatrix::getPixelNumber(uint8_t x, uint8_t y)
{
    return static_cast<uint16_t>(XY(y, x));
}

void MyMatrix::drawPixelXY(uint8_t x, uint8_t y, CRGB color)
{
    drawPixel(y, x, color);
}

void MyMatrix::drawLineXY(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, CRGB color)
{
    drawLine(y0, x0, y1, x1, color);
}

void MyMatrix::drawLineXY(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
    drawLine(y0, x0, y1, x1, color);
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

void MyMatrix::fillRectXY(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
    fillRect(y, x, h, w, color);
}
