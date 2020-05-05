#include "TwinklesEffect.h"

namespace {

uint8_t hue = 0;
CRGB* ledsbuff = nullptr;

// settings
uint8_t speed = 4;
uint8_t mult = 6;

}

TwinklesEffect::TwinklesEffect()
{

}

void TwinklesEffect::activate()
{
    hue = 0;
    ledsbuff = new CRGB[myMatrix->GetNumLeds()];
    for (uint32_t idx=0; idx < myMatrix->GetNumLeds(); idx++) {
        if (random8(settings.scale % 11) == 0) {
            ledsbuff[idx].r = random8();
            ledsbuff[idx].g = random8(1, speed * 2 +1);
            ledsbuff[idx].b = random8();
        } else {
            ledsbuff[idx] = 0;
        }
    }
}

void TwinklesEffect::deactivate()
{
    delete[] ledsbuff;
}

void TwinklesEffect::tick()
{
    for (uint16_t idx=0; idx < myMatrix->GetNumLeds(); idx++) {
        if (ledsbuff[idx].b == 0){
            if (random8(settings.scale % 11) == 0 && hue > 0) {
                ledsbuff[idx].r = random8();
                ledsbuff[idx].g = random8(1, speed + 1);
                ledsbuff[idx].b = ledsbuff[idx].g;
                hue--;
            }
        } else if (ledsbuff[idx].g <= speed) {
            if (ledsbuff[idx].b > 255 - ledsbuff[idx].g - mult) {
                ledsbuff[idx].b = 255;
                ledsbuff[idx].g = ledsbuff[idx].g + speed;
            } else {
                ledsbuff[idx].b = ledsbuff[idx].b + ledsbuff[idx].g + mult;
            }
        } else {
            if (ledsbuff[idx].b <= ledsbuff[idx].g - speed + mult) {
                ledsbuff[idx].b = 0;
                hue++;
            } else {
                ledsbuff[idx].b = ledsbuff[idx].b - ledsbuff[idx].g + speed - mult;
            }
        }
        if (ledsbuff[idx].b == 0) {
            myMatrix->setLed(idx, CRGB(0));
        } else {
            myMatrix->setLed(idx,
                             ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                         ledsbuff[idx].r,
                                         ledsbuff[idx].b));
        }
    }
}

void TwinklesEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("speed"))) {
        speed = json[F("speed")];
    }
    if (json.containsKey(F("mult"))) {
        mult = json[F("mult")];
    }
}

void TwinklesEffect::writeSettings(JsonObject &json)
{
    json[F("speed")] = speed;
    json[F("mult")] = mult;
}
