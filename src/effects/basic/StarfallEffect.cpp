#include "StarfallEffect.h"
#include <Spectrometer.h>

namespace  {

uint8_t tailStep = 100;
uint8_t saturation = 150;

bool useSpectrometer = false;

} // namespace

StarfallEffect::StarfallEffect(const String &id)
    : Effect(id)
{
}

void StarfallEffect::tick()
{

    for (uint8_t i = mySettings->matrixSettings.height / 2; i < mySettings->matrixSettings.height; i++) {
        if (!myMatrix->getPixColorXY(0, i)
                && (random(0, settings.scale) == 0)
                && !myMatrix->getPixColorXY(0, i + 1)
                && !myMatrix->getPixColorXY(0, i - 1)) {
            uint8_t hue = (mySettings->generalSettings.soundControl && useSpectrometer)
                    ? mySpectrometer->asHue()
                    : random(0, 200);
            myMatrix->drawPixelXY(0, i, CHSV(hue, saturation, 255));
        }
    }

    for (uint8_t i = 0; i < mySettings->matrixSettings.width / 2; i++) {
        if (!myMatrix->getPixColorXY(i, mySettings->matrixSettings.height - 1)
                && (random(0, settings.scale) == 0)
                && !myMatrix->getPixColorXY(i + 1, mySettings->matrixSettings.height - 1)
                && !myMatrix->getPixColorXY(i - 1, mySettings->matrixSettings.height - 1)) {
            uint8_t hue = (mySettings->generalSettings.soundControl && useSpectrometer)
                    ? mySpectrometer->asHue()
                    : random(0, 200);
            myMatrix->drawPixelXY(i, mySettings->matrixSettings.height - 1, CHSV(hue, saturation, 255));
        }
    }

    // сдвигаем по диагонали
    for (uint8_t y = 0; y < mySettings->matrixSettings.height - 1; y++) {
        for (uint8_t x = mySettings->matrixSettings.width - 1; x > 0; x--) {
            myMatrix->drawPixelXY(x, y, myMatrix->getPixColorXY(x - 1, y + 1));
        }
    }

    // уменьшаем яркость левой и верхней линии, формируем "хвосты"
    for (uint8_t i = mySettings->matrixSettings.height / 2; i < mySettings->matrixSettings.height; i++) {
        myMatrix->fadePixelXY(0, i, tailStep);
    }
    for (uint8_t i = 0; i < mySettings->matrixSettings.width / 2; i++) {
        myMatrix->fadePixelXY(i, mySettings->matrixSettings.height - 1, tailStep);
    }
}

void StarfallEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
//    if (json.containsKey(F("useSpectrometer"))) {
//        useSpectrometer = json[F("useSpectrometer")];
//    }
    if (json.containsKey(F("step"))) {
        tailStep = json[F("step")];
    }
    if (json.containsKey(F("sat"))) {
        saturation = json[F("sat")];
    }
}

void StarfallEffect::writeSettings(JsonObject &json)
{
//    json[F("useSpectrometer")] = useSpectrometer;
    json[F("step")] = tailStep;
    json[F("sat")] = saturation;
}
