#include "ColorEffect.h"
#include <Spectrometer.h>

namespace  {

bool useSpectrometer = false;
uint32_t myColor = 0;

} // namespace

ColorEffect::ColorEffect()
{
}

void ColorEffect::tick()
{
    if (myColor > 0) {
        myMatrix->fill(CRGB(myColor));
    } else {
        uint8_t hue = (mySettings->generalSettings.soundControl && useSpectrometer)
                ? mySpectrometer->asHue()
                : settings.scale * 2.55;
        myMatrix->fill(CHSV(hue, 255, 255));
    }
}

void ColorEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
//    if (json.containsKey(F("useSpectrometer"))) {
//        useSpectrometer = json[F("useSpectrometer")];
//    }
    if (json.containsKey(F("color"))) {
        myColor = json[F("color")];
    }
}

void ColorEffect::writeSettings(JsonObject &json)
{
//    json[F("useSpectrometer")] = useSpectrometer;
    json[F("color")] = myColor;
}
