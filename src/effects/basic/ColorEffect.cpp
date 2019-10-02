#include "ColorEffect.h"
#include <Spectrometer.h>

namespace  {

bool useSpectrometer = false;

} // namespace

ColorEffect::ColorEffect()
{
}

void ColorEffect::tick()
{
    uint8_t hue = (mySettings->generalSettings.soundControl && useSpectrometer)
            ? mySpectrometer->asHue()
            : settings.scale * 2.55;
    myMatrix->fill(CHSV(hue, 255, 255));
}

void ColorEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("useSpectrometer"))) {
        useSpectrometer = json[F("useSpectrometer")];
    }
}

void ColorEffect::writeSettings(JsonObject &json)
{
    json[F("useSpectrometer")] = useSpectrometer;
}
