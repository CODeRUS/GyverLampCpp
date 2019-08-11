#include "ClockEffect.h"
#include "GyverTimer.h"

namespace  {

uint8_t delayer = 0;
bool hours = true;

String getClockTime()
{
    return GyverTimer::Hours() + ":" + GyverTimer::Minutes();
}

} // namespace

ClockEffect::ClockEffect()
{
    effectName = "Clock vertical";

    settings = new Settings::EffectSettings();
    settings->effectScale = 1;
    settings->effectSpeed = 250;
    settings->effectBrightness = 10;
}

void ClockEffect::tick()
{
    if (delayer < 7) {
        ++delayer;
        return;
    }

    myMatrix->clear();
    if (hours) {
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
        myMatrix->setCursor(0, 0);
    } else {
        myMatrix->setTextColor(myMatrix->Color(30, 60, 30));
        myMatrix->setCursor(-13, 0);
    }
    myMatrix->print(getClockTime());
    myMatrix->show();
    delay(100);
    hours = !hours;
    delayer = 0;
}

void ClockEffect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
}

void ClockEffect::deactivate()
{
}
