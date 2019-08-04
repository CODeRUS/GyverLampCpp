#include "ClockHorizontal2Effect.h"
#include "GyverTimer.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

} // namespace

ClockHorizontal2Effect::ClockHorizontal2Effect()
{
    effectName = "Clock horizontal";

    settings = new Settings::EffectSettings();
    settings->effectScale = 1;
    settings->effectSpeed = 250;
    settings->effectBrightness = 10;
}

void ClockHorizontal2Effect::tick()
{
    myMatrix->clear();

    if (--posx == -6) {
        posx = 0;
        if (++indexx == 6) {
            indexx = 0;
        }
    }

    const String clockTime = GyverTimer::ClockTime();
    String time = clockTime.substring(indexx);
    if (indexx > 0) {
        time += clockTime.substring(0, indexx);
    }

    myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
    myMatrix->setCursor(posx, 0);
    myMatrix->print(time);

    const String clockTime2 = clockTime.substring(3) + clockTime.substring(0, 3);
    String time2 = clockTime2.substring(indexx);
    if (indexx > 0) {
        time2 += clockTime2.substring(0, indexx);
    }

    myMatrix->setCursor(posx, 8);
    myMatrix->print(time2);

    delay(1);
    myMatrix->show();
}

void ClockHorizontal2Effect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(myMatrix->Color(40, 40, 40));

    if (myMatrix->getRotation() != 0) { // for horizontal only
        myMatrix->setRotation(0);
    }
}

void ClockHorizontal2Effect::deactivate()
{
    GyverTimer::SetInterval(0);
    if (myMatrix->getRotation() != 3) {
        myMatrix->setRotation(3);
    }
}
