#include "TwirlRainbowEffect.h"

namespace {

uint8_t hue = 0;
uint8_t hueStep = 8;
uint16_t twirlFactor = 300;

uint8_t maxDimension = 0;
float ratio = 0.0f;

}

TwirlRainbowEffect::TwirlRainbowEffect(const String &id)
    : Effect(id)
{

}

void TwirlRainbowEffect::activate()
{
    maxDimension = max(mySettings->matrixSettings.width, mySettings->matrixSettings.height);
    ratio = static_cast<float>(mySettings->matrixSettings.width) / mySettings->matrixSettings.height;
}

void TwirlRainbowEffect::tick()
{
    hue += hueStep;
    float factor = (twirlFactor / 100.0f) * (settings.scale / 100.0f);
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        for (uint8_t y = 0; y < mySettings->matrixSettings.height; y++) {
            CRGB thisColor = CHSV(static_cast<uint8_t>(hue + (ratio * x + y * factor) * (255.0f / maxDimension)), 255, 255);
            myMatrix->drawPixelXY(x, y, thisColor);
        }
    }
}

void TwirlRainbowEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("step"))) {
        hueStep = json[F("step")];
    }
    if (json.containsKey(F("twirl"))) {
        twirlFactor = json[F("twirl")];
    }
}

void TwirlRainbowEffect::writeSettings(JsonObject &json)
{
    json[F("step")] = hueStep;
    json[F("twirl")] = twirlFactor;
}
