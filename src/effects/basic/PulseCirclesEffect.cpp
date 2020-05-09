#include "PulseCirclesEffect.h"

namespace {

CRGBPalette16 palette = RainbowColors_p;
const uint8_t limitSteps = 6U;
const float fadeRate = 0.8f;
uint8_t step = 0;
uint8_t deltaHue, deltaHue2 = 0;
uint8_t hue, hue2 = 0;

uint8_t mode = 1;
uint8_t currentRadius = 4;
uint8_t pulse_centerX = 0;
uint8_t pulse_centerY = 0;

void _drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, const CRGB & color) {
    uint8_t a = radius, b = 0;
    int radiusError = 1 - a;

    if (radius == 0) {
        myMatrix->drawPixelXY(x0, y0, color);
        return;
    }

    while (a >= b)  {
        myMatrix->drawPixelXY(a + x0, b + y0, color);
        myMatrix->drawPixelXY(b + x0, a + y0, color);
        myMatrix->drawPixelXY(-a + x0, b + y0, color);
        myMatrix->drawPixelXY(-b + x0, a + y0, color);
        myMatrix->drawPixelXY(-a + x0, -b + y0, color);
        myMatrix->drawPixelXY(-b + x0, -a + y0, color);
        myMatrix->drawPixelXY(a + x0, -b + y0, color);
        myMatrix->drawPixelXY(b + x0, -a + y0, color);
        b++;
        if (radiusError < 0) {
            radiusError += 2 * b + 1;
        } else {
            a--;
            radiusError += 2 * (b - a + 1);
        }
    }
}

}

PulseCirclesEffect::PulseCirclesEffect()
{

}

void PulseCirclesEffect::tick()
{
    myMatrix->dimAll(248u);
    uint8_t _sat;
    if (step <= currentRadius) {
        for (uint8_t i = 0; i < step; i++ ) {
            uint8_t _dark = qmul8( 2U, cos8 (128U / (step + 1U) * (i + 1U)));
            CRGB _pulse_color;
            switch (mode) {
            case 1U: // random circles
                _pulse_color = CHSV(hue, 255U, _dark);
                break;
            case 2U: // changing colored circles
                deltaHue2 = settings.scale;
                _pulse_color = CHSV(hue2, 255U, _dark);
                break;
            case 3U:  // manual colored circles
                deltaHue = settings.scale * 2.55;
                _pulse_color = CHSV(deltaHue, 255U, _dark);
                break;
            case 4U: // rainbow colored circles
                deltaHue += settings.scale;
                _pulse_color = CHSV(deltaHue, 255U, _dark);
                break;
            case 5U: // manual rainbow colored circles
                _sat =  qsub8( 255U, cos8 (128U / (step + 1U) * (i + 1U))) ;
                deltaHue += settings.scale;
                _pulse_color = CHSV(deltaHue, _sat, _dark);
                break;
            case 6U: // manual colored bubbles
                _sat =  qsub8( 255U, cos8 (128U / (step + 1U) * (i + 1U))) ;
                deltaHue = settings.scale * 2.55;
                _pulse_color = CHSV(deltaHue, _sat, _dark);
                break;
            case 7U: // changing colored bubbles
                _sat =  qsub8( 255U, cos8 (128U / (step + 1U) * (i + 1U))) ;
                deltaHue2 = settings.scale;
                _pulse_color = CHSV(hue2, _sat, _dark);
                break;
            case 8U: // random bubbles
                _sat =  qsub8( 255U, cos8 (128U / (step + 1U) * (i + 1U))) ;
                deltaHue = hue;
                _pulse_color = CHSV(deltaHue, _sat, _dark);
                break;
            }
            _drawCircle(pulse_centerX, pulse_centerY, i, _pulse_color);
        }
    } else {
        pulse_centerX = random8(mySettings->matrixSettings.width - 5U) + 3U;
        pulse_centerY = random8(mySettings->matrixSettings.height - 5U) + 3U;
        hue2 += deltaHue2;
        hue = random8(0U, 255U);
        currentRadius = random8(3U, 9U);
        step = 0;
    }
    step++;
}

void PulseCirclesEffect::activate()
{
    pulse_centerX = random8(mySettings->matrixSettings.width - 5U) + 3U;
    pulse_centerY = random8(mySettings->matrixSettings.height - 5U) + 3U;
}

void PulseCirclesEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("m"))) {
        mode = json[F("m")];
    }
}

void PulseCirclesEffect::writeSettings(JsonObject &json)
{
    json[F("m")] = mode;
}
