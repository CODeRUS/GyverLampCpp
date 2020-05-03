#include "MyMatrix.h"
#include "Settings.h"

namespace  {

uint8_t defaultWidth = 16;
uint8_t defaultHeight = 16;
uint8_t defaultType = NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
                      NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG;

uint8_t defaultMaxBrightness = 80;
uint32_t defaultCurrentLimit = 1000;

uint8_t defaultRoatation = 3;

#if defined(SONOFF)
const uint8_t ledPin = 14;
#elif defined(ESP32)
const uint8_t ledPin = 13;
#else
const uint8_t ledPin = D4;
#endif

uint16_t numLeds = 0;

CRGB* leds = nullptr;

MyMatrix *instance = nullptr;

const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {
  0x000000, 0x060707, 0x101110, 0x151717,
  0x1C1D22, 0x242A28, 0x363B3A, 0x313634,
  0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1,
  0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9
};

const TProgmemRGBPalette16 *palette_arr[] = {
    &PartyColors_p,
    &OceanColors_p,
    &LavaColors_p,
    &HeatColors_p,
    &WaterfallColors_p,
    &CloudColors_p,
    &ForestColors_p,
    &RainbowColors_p,
    &RainbowStripeColors_p
};

} // namespace

FASTLED_NAMESPACE_BEGIN
uint16_t XY(uint8_t x, uint8_t y) {
    return myMatrix->XY(x, y);
}
FASTLED_NAMESPACE_END

MyMatrix *MyMatrix::Instance()
{
    return instance;
}

void MyMatrix::Initialize()
{
    if (instance) {
        return;
    }

    uint8_t sizeWidth = mySettings->matrixSettings.width;
    uint8_t sizeHeight = mySettings->matrixSettings.height;
    uint8_t matrixType = mySettings->matrixSettings.type;

    numLeds = sizeWidth * sizeHeight;
    leds = new CRGB[numLeds]();
    FastLED.addLeds<WS2812B, ledPin, GRB>(leds, numLeds);

    uint8_t maxBrightness = mySettings->matrixSettings.maxBrightness;
    Serial.printf_P(PSTR("Set max brightness to: %u\n"), maxBrightness);
    FastLED.setBrightness(maxBrightness);

    uint32_t currentLimit = mySettings->matrixSettings.currentLimit;
    Serial.printf_P(PSTR("Set current limit to: %u\n"), currentLimit);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, currentLimit);

    instance = new MyMatrix(leds, sizeWidth, sizeHeight, matrixType);
    uint8_t rotation = GetRotation();
    Serial.printf_P(PSTR("Set rotation to: %u\n"), rotation);
    instance->setRotation(rotation);

    instance->begin();
    instance->clear();
    instance->show();
}

uint16_t MyMatrix::GetNumLeds()
{
    return numLeds;
}

uint8_t MyMatrix::GetRotation()
{
    uint8_t rotation = mySettings->matrixSettings.rotation;
    return rotation;
}

const TProgmemRGBPalette16 *MyMatrix::GetPalette(uint8_t pct)
{
    return palette_arr[(uint8_t)(pct / 100.0f * ((sizeof(palette_arr) / sizeof(TProgmemRGBPalette16 *)) - 0.01f))];
}

uint8_t MyMatrix::GetCenterX()
{
    return mySettings->matrixSettings.width / 2 - 1;
}

uint8_t MyMatrix::GetCenterY()
{
    return mySettings->matrixSettings.height / 2 - 1;
}

CRGB *MyMatrix::GetLeds()
{
    return leds;
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

void MyMatrix::fillProgress(double progress)
{
    FastLED.clear();

    const uint16_t number = static_cast<uint16_t>(numLeds * progress);
    const uint8_t fullRows = static_cast<uint8_t>(number / width());
    for (uint8_t y = 0; y < fullRows; ++y) {
        for (uint8_t x = 0; x < width(); ++x) {
            drawPixelXY(x, y, CRGB(5, 5, 5));
        }
    }

    const uint8_t remainingProgress = static_cast<uint8_t>(number % width());
    for (uint8_t x = 0; x < remainingProgress; ++x) {
        drawPixelXY(x, fullRows, CRGB(5, 5, 5));
    }

    const String percent = String(static_cast<int>(progress * 100));
    setCursor(0, 5);
    setPassThruColor(CRGB(40, 0, 0));
    print(percent);
    setPassThruColor();
    delay(1);

    FastLED.show();
}

void MyMatrix::setLed(uint16_t index, CRGB color)
{
    leds[index] = color;
}

void MyMatrix::fadeToBlackBy(uint16_t index, uint8_t step)
{
    leds[index].fadeToBlackBy(step);
}

void MyMatrix::dimAll(uint8_t value)
{
    for (uint16_t i = 0; i < numLeds; i++) {
        leds[i].nscale8(value);
    }
}

uint8_t MyMatrix::wrapX(int8_t x)
{
    return (x + width()) % width();
}

uint8_t MyMatrix::wrapY(int8_t y)
{
    return (y + height()) % height();
}

void MyMatrix::fader(uint8_t step)
{
    for (uint8_t x = 0; x < width(); x++) {
        for (uint8_t y = 0U; y < height(); y++) {
            fadePixel(x, y, step);
        }
    }
}

void MyMatrix::fadePixel(uint8_t x, uint8_t y, uint8_t step)
{
    uint16_t pixelNum = XY(x, y);
    if (!getPixColor(pixelNum)) {
        return;
    }

    if (leds[pixelNum].r >= 30U ||
            leds[pixelNum].g >= 30U ||
            leds[pixelNum].b >= 30U) {
        leds[pixelNum].fadeToBlackBy(step);
    } else {
        leds[pixelNum] = 0U;
    }
}

uint32_t MyMatrix::colorcode(const CRGB &color)
{
    return uint32_t{color.r} << 16 |
           uint32_t{color.g} << 8 |
uint32_t{color.b};
}

void MyMatrix::applyBlur2d(uint8_t amount)
{
    blur2d(leds, width(), height(), amount);
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

void MyMatrix::clear(bool shouldShow)
{
    FastLED.clear();
    if (shouldShow) {
        delay(1);
        show();
    }
}

uint16_t MyMatrix::getPixelNumberXY(uint8_t x, uint8_t y)
{
    return static_cast<uint16_t>(XY(y, x));
}

void MyMatrix::drawPixelXY(uint8_t x, uint8_t y, CRGB color)
{
    leds[myMatrix->getPixelNumberXY(x, y)] = color;
}

void MyMatrix::drawLineXY(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, CRGB color)
{
    setPassThruColor(color);
    drawLine(y0, x0, y1, x1, 0);
    setPassThruColor();
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
    return getPixColor(getPixelNumberXY(x, y));
}

void MyMatrix::fillRectXY(uint8_t x, uint8_t y, uint8_t w, uint8_t h, CRGB color)
{
    setPassThruColor(color);
    fillRect(y, x, h, w, 0);
    setPassThruColor();
}

void MyMatrix::fadePixelXY(uint8_t x, uint8_t y, uint8_t step)
{
    const uint16_t pixelNum = myMatrix->getPixelNumberXY(x, y);
    const CRGB color = myMatrix->getPixColor(pixelNum);

    if (!color) {
        return;
    }

    if (color.r >= 30 || color.g >= 30 || color.b >= 30) {
        myMatrix->fadeToBlackBy(pixelNum, step);
    } else {
        myMatrix->setLed(pixelNum, CRGB::Black);
    }
}
