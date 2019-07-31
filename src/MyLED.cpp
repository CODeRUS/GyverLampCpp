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

uint8_t connectionWidth = 0;
uint8_t connectionHeight = 0;

uint16_t numLeds = 0;

CRGB* leds = nullptr;

uint8_t segments = 1;
MyLED::MatrixType matrixType = MyLED::MatrixTypeZigzag;
MyLED::MatrixConnectionPoint connectionPoint = MyLED::MatrixConnectionPointRightDown;
MyLED::MatrixDirection matrixDirection = MyLED::MatrixDirectionCounterClockwise;

const uint32_t defaultMaxCurrent = 1000;

} // namespace

void MyLED::Initialize(uint8_t sizeWidth, uint8_t sizeHeight, MyLED::MatrixType mType, MyLED::MatrixConnectionPoint cPoint, MyLED::MatrixDirection mDirection, uint8_t cSegments)
{
    width = sizeWidth;
    height = sizeHeight;
    Serial.printf("Matrix initialization. width: %u, height: %u\n", width, height);
    numLeds = width * height;
    Serial.printf("Total leds: %u\n", numLeds);

    segments = cSegments;
    Serial.printf("Segments count: %u\n", segments);

    matrixType = mType;
    connectionPoint = cPoint;
    matrixDirection = mDirection;

    Serial.printf("Matrix type: %d\n", matrixType);
    Serial.printf("Connection point: %d\n", connectionPoint);
    Serial.printf("Matrix direction: %d\n", matrixDirection);

    switch (connectionPoint) {
    case MatrixConnectionPointLeftDown:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            connectionWidth = width;
            connectionHeight = height;
        } else {
            connectionWidth = height;
            connectionHeight = width;
        }
        break;
    case MatrixConnectionPointLeftUp:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            connectionWidth = height;
            connectionHeight = width;
        } else {
            connectionWidth = width;
            connectionHeight = height;
        }
        break;
    case MatrixConnectionPointRightUp:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            connectionWidth = width;
            connectionHeight = height;
        } else {
            connectionWidth = height;
            connectionHeight = width;
        }
        break;
    case MatrixConnectionPointRightDown:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            connectionWidth = height;
            connectionHeight = width;
        } else {
            connectionWidth = width;
            connectionHeight = height;
        }
        break;
    }

    leds = new CRGB[numLeds]();
    FastLED.addLeds<WS2812B, ledPin, GRB>(leds, numLeds);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, defaultMaxCurrent);
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
    return connectionWidth;
}

uint8_t MyLED::getHeight()
{
    return connectionHeight;
}

void MyLED::show()
{
    FastLED.show();
}

uint16_t MyLED::getPixelNumber(uint8_t x, uint8_t y)
{
    uint8_t posx = x;
    uint8_t posy = y;

    switch (connectionPoint) {
    case MatrixConnectionPointLeftDown:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            posx = x;
            posy = y;
        } else {
            posx = y;
            posy = x;
        }
        break;
    case MatrixConnectionPointLeftUp:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            posx = height - y - 1;
            posy = x;
        } else {
            posx = x;
            posy = height - y - 1;
        }
        break;
    case MatrixConnectionPointRightUp:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            posx = width - x - 1;
            posy = height - y - 1;
        } else {
            posx = height - y - 1;
            posy = width - x - 1;
        }
        break;
    case MatrixConnectionPointRightDown:
        if (matrixDirection == MatrixDirectionCounterClockwise) {
            posx = y;
            posy = width - x - 1;
        } else {
            posx = width - x - 1;
            posy = y;
        }
        break;
    }

    if ((posy % 2 == 0) || matrixType == MatrixTypeParallel) {
        return (posy * connectionWidth + posx);
    } else {
        return (posy * connectionWidth + connectionWidth - posx - 1);
    }
}

void MyLED::drawPixelXY(uint8_t x, uint8_t y, CRGB color)
{
    if (x >= connectionWidth || y >= connectionHeight) {
        Serial.printf("Warning! MyLED::drawPixelXY(%u, %u) values are incorrect!\n", x, y);
        return;
    }
    uint32_t thisPixel = getPixelNumber(x, y) * segments;
    for (uint8_t i = 0; i < segments; i++) {
        leds[thisPixel + i] = color;
    }
}

CRGB MyLED::getPixColor(uint16_t thisSegm)
{
    uint32_t thisPixel = thisSegm * segments;
    if (thisPixel > numLeds - 1) {
        return 0;
    }
    return leds[thisPixel];
}

CRGB MyLED::getPixColorXY(uint8_t x, uint8_t y)
{
    return getPixColor(getPixelNumber(x, y));
}
