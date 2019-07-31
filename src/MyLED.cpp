#include "MyLED.h"

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

uint8_t width = 0;
uint8_t height = 0;

uint16_t numLeds = 0;

CRGB* leds = nullptr;
FastLED_NeoMatrix *matrix = nullptr;

const uint32_t defaultMaxCurrent = 1000;

} // namespace

void MyLED::Initialize(uint8_t sizeWidth, uint8_t sizeHeight, uint8_t matrixType)
{
    width = sizeWidth;
    height = sizeHeight;
    Serial.printf("Matrix initialization. width: %u, height: %u\n", width, height);
    numLeds = width * height;
    Serial.printf("Total leds: %u\n", numLeds);

    leds = new CRGB[numLeds]();
    matrix = new FastLED_NeoMatrix(leds, width, height, 1, 1, matrixType);
    FastLED.addLeds<WS2812B, ledPin, GRB>(leds, numLeds);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, defaultMaxCurrent);
    matrix->begin();
    clear();
    show();
}

void MyLED::setCurrentLimit(uint32_t maxCurrent)
{
    FastLED.setMaxPowerInVoltsAndMilliamps(5, maxCurrent);
}

void MyLED::setBrightness(uint8_t brightness)
{
    FastLED.setBrightness(brightness);
}

void MyLED::clear()
{
    FastLED.clear();
}

void MyLED::fill(CRGB color, bool show)
{
    fill_solid(leds, numLeds, color);
    if (show) {
        FastLED.show();
    }
}

void MyLED::fillProgress(double progress, CRGB color)
{
    const uint16_t number = static_cast<uint16_t>(numLeds * progress);
    fill_solid(leds, number, color);
    FastLED.show();
}

void MyLED::setLed(uint8_t x, uint8_t y, CRGB color, bool show)
{
    setLed(getPixelNumber(x, y), color);
}

void MyLED::setLed(uint16_t index, CRGB color, bool show)
{
    leds[index] = color;
    if (show) {
        FastLED.show();
    }
}

void MyLED::fadeToBlackBy(uint16_t index, uint8_t step)
{
    leds[index].fadeToBlackBy(step);
}

uint8_t MyLED::getWidth()
{
    return static_cast<uint8_t>(matrix->width());
}

uint8_t MyLED::getHeight()
{
    return static_cast<uint8_t>(matrix->height());
}

void MyLED::matrixTest()
{
    for (uint8_t xx = 0; xx < width; xx++) {
        matrix->clear();
        matrix->drawPixel(xx, 0, CRGB(CRGB::Red));
        matrix->show();

        delay(100);
    }

    for (uint8_t yy = 0; yy < height; yy++) {
        matrix->clear();
        matrix->drawPixel(0, yy, CRGB(CRGB::Green));
        matrix->show();

        delay(100);
    }

    matrix->clear();
    matrix->show();
}

void MyLED::show()
{
    FastLED.show();
}

uint16_t MyLED::getPixelNumber(uint8_t x, uint8_t y)
{
    return static_cast<uint16_t>(matrix->XY(x, y));
}

void MyLED::drawPixelXY(uint8_t x, uint8_t y, CRGB color)
{
    matrix->drawPixel(x, y, color);
}

CRGB MyLED::getPixColor(uint16_t number)
{
    if (number > numLeds - 1) {
        return 0;
    }
    return leds[number];
}

CRGB MyLED::getPixColorXY(uint8_t x, uint8_t y)
{
    return getPixColor(getPixelNumber(x, y));
}
