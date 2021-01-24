#include "SparklesEffect.h"

namespace {

} // namespace

SparklesEffect::SparklesEffect(const String &id)
    : Effect(id)
{
}

void SparklesEffect::tick()
{
    for (uint8_t i = 0; i < settings.scale; i++) {
        uint8_t x = random(0, mySettings->matrixSettings.width);
        uint8_t y = random(0, mySettings->matrixSettings.height);
        if (!myMatrix->getPixColorXY(x, y)) {
            myMatrix->drawPixelXY(x, y, CHSV(random(0, 255), 255, 255));
        }
    }
    myMatrix->fader(70);
}
