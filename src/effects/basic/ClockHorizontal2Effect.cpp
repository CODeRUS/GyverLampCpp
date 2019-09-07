#include "ClockHorizontal2Effect.h"
#include "GyverTimer.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

String getClockTime1()
{
    return GyverTimer::Hours() + ":" + GyverTimer::Hours() + ":";
}

String getClockTime2()
{
    return GyverTimer::Minutes() + " " + GyverTimer::Minutes() + " ";
}

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

    const String clockTime = getClockTime1();
    if (--posx == -6) {
        posx = 0;
        if (++indexx == clockTime.length()) {
            indexx = 0;
        }
    }

    String time = clockTime.substring(indexx);
    if (indexx > 0) {
        time += clockTime.substring(0, indexx);
    }

    myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
    myMatrix->setCursor(posx, 0);
    myMatrix->print(time);

    const String clockTime2 = getClockTime2();
    String time2 = clockTime2.substring(indexx);
    if (indexx > 0) {
        time2 += clockTime2.substring(0, indexx);
    }

    myMatrix->setCursor(posx, 8);
    myMatrix->setTextColor(myMatrix->Color(30, 60, 30));
    myMatrix->print(time2);

    delay(1);
    myMatrix->show();
}

void ClockHorizontal2Effect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);

    int horizontalRotation = mySettings->matrixRotation - 3;
    if (horizontalRotation < 0) {
        horizontalRotation = horizontalRotation + 4;
    }

    if (myMatrix->getRotation() != horizontalRotation) { // for horizontal only
        myMatrix->setRotation(horizontalRotation);
    }
}

void ClockHorizontal2Effect::deactivate()
{
    GyverTimer::SetInterval(0);
    if (myMatrix->getRotation() != mySettings->matrixRotation) {
        myMatrix->setRotation(mySettings->matrixRotation);
    }
}
