#include "StormEffect.h"

namespace {

bool isColored = false;
uint8_t snowDensity = 32;

uint8_t saturation = 0;
uint8_t tailStep = 127;

}

StormEffect::StormEffect()
{

}

void StormEffect::tick()
{
    if (isColored) {
        saturation = settings.scale * 2.55;
    } else {
        tailStep = 255 - settings.scale * 2.55;
    }
    for (int8_t x = 0; x < mySettings->matrixSettings.width - 1; x++) {
        if (!random8(snowDensity) &&
                !myMatrix->getPixColorXY(myMatrix->wrapX(x),
                                         mySettings->matrixSettings.height - 1) &&
                !myMatrix->getPixColorXY(myMatrix->wrapX(x + 1),
                                         mySettings->matrixSettings.height - 1) &&
                !myMatrix->getPixColorXY(myMatrix->wrapX(x - 1),
                                         mySettings->matrixSettings.height - 1)) {
            myMatrix->drawPixelXY(x,
                                  mySettings->matrixSettings.height - 1,
                                  CHSV(random8(), saturation, random8(64, 255)));
        }
    }

    for (int8_t y = 0; y < mySettings->matrixSettings.height - 1; y++) {
        for (int8_t x = 0; x < mySettings->matrixSettings.width; x++) {
            myMatrix->drawPixelXY(myMatrix->wrapX(x + 1),
                                  y,
                                  myMatrix->getPixColorXY(x, y + 1));
        }
    }

    for (int8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        myMatrix->fadePixelXY(x,
                            mySettings->matrixSettings.height - 1,
                            tailStep);
    }
}

void StormEffect::writeSettings(JsonObject &json)
{
    json[F("colored")] = isColored;
    json[F("density")] = snowDensity;
}

void StormEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("colored"))) {
        isColored = json[F("colored")];
    }
    if (json.containsKey(F("density"))) {
        snowDensity = json[F("density")];
    }
}
