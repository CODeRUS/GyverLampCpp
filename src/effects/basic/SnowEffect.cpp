#include "SnowEffect.h"
#include <Spectrometer.h>

namespace  {

bool useSpectrometer = false;
uint32_t myColor = 0xffffff;

} // namespace

SnowEffect::SnowEffect(const String &id)
    : Effect(id)
{
}

void SnowEffect::tick()
{
    // сдвигаем всё вниз
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        for (byte y = 0; y < mySettings->matrixSettings.height - 1; y++) {
            myMatrix->drawPixelXY(x, y, myMatrix->getPixColorXY(x, y + 1));
        }
    }

    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        // заполняем случайно верхнюю строку
        // а также не даём двум блокам по вертикали вместе быть
        if (!myMatrix->getPixColorXY(x, mySettings->matrixSettings.height - 2) && (random(0, settings.scale) == 0)) {
            CRGB color = (mySettings->generalSettings.soundControl && useSpectrometer)
                    ? CHSV(mySpectrometer->asHue(), mySpectrometer->asHue(), 255)
                    : CRGB(myColor);
            myMatrix->drawPixelXY(x, mySettings->matrixSettings.height - 1, color);
        } else {
            myMatrix->drawPixelXY(x, mySettings->matrixSettings.height - 1, 0x000000);
        }
    }
}

void SnowEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
//    if (json.containsKey(F("useSpectrometer"))) {
//        useSpectrometer = json[F("useSpectrometer")];
//    }
    if (json.containsKey(F("color"))) {
        const JsonVariant color = json[F("color")];
        if (color.is<uint32_t>()) {
            myColor = json[F("color")];
        } else if (color.is<JsonObject>()) {
            myColor = json[F("color")]["r"].as<uint8_t>() << 16 |
                      json[F("color")]["g"].as<uint8_t>() << 8 |
                      json[F("color")]["b"].as<uint8_t>();
        }
    }
}

void SnowEffect::writeSettings(JsonObject &json)
{
//    json[F("useSpectrometer")] = useSpectrometer;
    JsonObject color = json.createNestedObject("color");
    color["r"] = (uint8_t)(myColor >> 16);
    color["g"] = (uint8_t)(myColor >> 8);
    color["b"] = (uint8_t)(myColor);
}
