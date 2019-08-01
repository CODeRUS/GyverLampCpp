#include "ClockEffect.h"
#include "GyverTimer.h"

namespace  {

int16_t x = 0;
uint8_t delayer = 0;
int stepper = -1;

} // namespace

ClockEffect::ClockEffect()
{
    effectName = "Clock";

    settings = new Settings::EffectSettings();
    settings->effectScale = 1;
    settings->effectSpeed = 250;
    settings->effectBrightness = 10;
}

void ClockEffect::tick()
{
    if (delayer < 3) {
        ++delayer;
        return;
    }

    myMatrix->clear();
    myMatrix->setCursor(x, 0);
    myMatrix->print(GyverTimer::ClockTime());
    myMatrix->show();
    x += stepper;
    if (x < -12) {
        stepper = 1;
    } else if (x == 0) {
        stepper = -1;
    }
    delay(1);
    delayer = 0;
}

void ClockEffect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(myMatrix->Color(60, 60, 60));
    myMatrix->setRotation(1);
}

void ClockEffect::deactivate()
{
    GyverTimer::SetInterval(0);
    myMatrix->setRotation(0);
}
