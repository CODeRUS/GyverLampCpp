#include "MyMatrix.h"
#include "Settings.h"

namespace  {

#if defined(SONOFF)
const uint8_t ledPin = 14;
#elif defined(ESP32)
const uint8_t ledPin = 13;
#else
const uint8_t ledPin = D4;
#endif

uint16_t numLeds = 0;

CRGB* leds = nullptr;

MyMatrix *object = nullptr;

const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {
  0x000000, 0x060707, 0x101110, 0x151717,
  0x1C1D22, 0x242A28, 0x363B3A, 0x313634,
  0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1,
  0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9
};

const TProgmemRGBPalette16 *colorPallettes[] = {
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

const CRGBPalette16 WoodFireColors_p = {
    CRGB::Black, CRGB::OrangeRed, CRGB::Orange, CRGB::Gold
};
const CRGBPalette16 NormalFire_p = {
    CRGB::Black, CRGB::Red, 0xff3c00, 0xff7800
};
const CRGBPalette16 NormalFire2_p = {
    CRGB::Black, CRGB::FireBrick, 0xff3c00, 0xff7800
};
const CRGBPalette16 SodiumFireColors_p = {
    CRGB::Black, CRGB::Orange, CRGB::Gold, CRGB::Goldenrod
};
const CRGBPalette16 CopperFireColors_p = {
    CRGB::Black, CRGB::Green, CRGB::GreenYellow, CRGB::LimeGreen
};
const CRGBPalette16 AlcoholFireColors_p = {
    CRGB::Black, CRGB::Blue, CRGB::DeepSkyBlue, CRGB::LightSkyBlue
};
const CRGBPalette16 RubidiumFireColors_p = {
    CRGB::Black, CRGB::Indigo, CRGB::Indigo, CRGB::DarkBlue
};
const CRGBPalette16 PotassiumFireColors_p = {
    CRGB::Indigo, CRGB::MediumPurple, CRGB::DeepPink
};
const CRGBPalette16 LithiumFireColors_p = {
    CRGB::Black, CRGB::FireBrick, CRGB::Pink, CRGB::DeepPink
};
const CRGBPalette16 HeatColors_x = HeatColors_p;
const CRGBPalette16 *firePalettes[] = {
  &HeatColors_x,
  &WoodFireColors_p,
  &NormalFire_p,
  &NormalFire2_p,
  &LithiumFireColors_p,
  &SodiumFireColors_p,
  &CopperFireColors_p,
  &AlcoholFireColors_p,
  &RubidiumFireColors_p,
  &PotassiumFireColors_p
};

} // namespace

FASTLED_NAMESPACE_BEGIN
uint16_t XY(uint8_t x, uint8_t y) {
    return object->XY(x, y);
}
FASTLED_NAMESPACE_END

MyMatrix *MyMatrix::instance()
{
    return object;
}

void MyMatrix::Initialize()
{
    if (object) {
        return;
    }

    Serial.println(F("Initializing MyMatrix"));

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

    object = new MyMatrix(leds, sizeWidth, sizeHeight, matrixType);
    uint8_t rotation = mySettings->matrixSettings.rotation;
    Serial.printf_P(PSTR("Set rotation to: %u\n"), rotation);
    object->setRotation(rotation);

    object->begin();
    object->clear();
    object->show();
}

uint8_t MyMatrix::getRotation()
{
    return rotation;
}

uint16_t MyMatrix::getNumLeds()
{
    return numLeds;
}

const TProgmemRGBPalette16 *MyMatrix::GetColorPalette(uint8_t pct)
{
    return colorPallettes[(uint8_t)(pct / 100.0f * ((sizeof(colorPallettes) / sizeof(TProgmemRGBPalette16 *)) - 0.01f))];
}

const CRGBPalette16 *MyMatrix::GetFirePalette(uint8_t pct)
{
    return firePalettes[(uint8_t)(pct / 100.0f * ((sizeof(firePalettes) / sizeof(CRGBPalette16 *)) - 0.01f))];
}

uint8_t MyMatrix::getCenterX()
{
    return mySettings->matrixSettings.width / 2 - 1;
}

uint8_t MyMatrix::getCenterY()
{
    return mySettings->matrixSettings.height / 2 - 1;
}

uint8_t MyMatrix::getDimension()
{
    if (type & NEO_MATRIX_ROWS) {
        return width();
    } else {
        return height();
    }
}

CRGB *MyMatrix::getLeds()
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

void MyMatrix::fadeToBlackBy(uint8_t step)
{
    for (uint16_t i = 0; i < numLeds; i++) {
        leds[i].fadeToBlackBy(step);
    }
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
        for (uint8_t y = 0; y < height(); y++) {
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

    if (leds[pixelNum].r >= 30 ||
            leds[pixelNum].g >= 30 ||
            leds[pixelNum].b >= 30) {
        leds[pixelNum].fadeToBlackBy(step);
    } else {
        leds[pixelNum] = 0;
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

void MyMatrix::tintPixelXY(uint8_t x, uint8_t y, CRGB color)
{
    leds[myMatrix->getPixelNumberXY(x, y)] += color;
}

void MyMatrix::shadePixelXY(uint8_t x, uint8_t y, CRGB color)
{
    leds[myMatrix->getPixelNumberXY(x, y)] -= color;
}

void MyMatrix::blendPixelXY(uint8_t x, uint8_t y, const CRGB &color, uint8_t amount)
{
    nblend(leds[myMatrix->getPixelNumberXY(x, y)], color, amount);
}

void MyMatrix::dimPixelXY(uint8_t x, uint8_t y, uint8_t value)
{
    leds[myMatrix->getPixelNumberXY(x, y)].nscale8(value);
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
