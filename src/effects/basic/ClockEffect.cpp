#include "ClockEffect.h"

namespace  {

int16_t x = 0;
uint8_t pass = 0;

const uint16_t colors[] = {
    myMatrix->Color(255, 0, 0),
    myMatrix->Color(0, 255, 0),
    myMatrix->Color(0, 0, 255)
};

} // namespace

ClockEffect::ClockEffect()
{
    effectName = "Clock";
    x = width;

    settings = new Settings::EffectSettings();
    settings->effectScale = 1;
    settings->effectSpeed = 100;
    settings->effectBrightness = 80;
}

void ClockEffect::tick()
{
    myMatrix->fillScreen(0);
    myMatrix->setCursor(x, 0);
    myMatrix->print(F("22:22"));
    if(--x < -36) {
        x = myMatrix->width();
        if(++pass >= 3) pass = 0;
        myMatrix->setTextColor(colors[pass]);
    }
    myMatrix->show();
}
