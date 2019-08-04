#include "ClockHorizontal1Effect.h"
#include "GyverTimer.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

} // namespace

ClockHorizontal1Effect::ClockHorizontal1Effect()
{
    effectName = "Clock horizontal colored";

    settings = new Settings::EffectSettings();
    settings->effectScale = 1;
    settings->effectSpeed = 250;
    settings->effectBrightness = 10;
}

void ClockHorizontal1Effect::tick()
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

    myMatrix->setCursor(posx, 0);
    const uint8_t dotIndex1 = time.indexOf(':');
    const uint8_t spaceIndex1 =  time.indexOf(' ');
    if (dotIndex1 < spaceIndex1) {
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
        myMatrix->print(time.substring(0, dotIndex1));
        myMatrix->setTextColor(myMatrix->Color(0, 40, 0));
        myMatrix->print(time.substring(dotIndex1, spaceIndex1));
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
        myMatrix->print(time.substring(spaceIndex1));
    } else {
        myMatrix->setTextColor(myMatrix->Color(0, 40, 0));
        myMatrix->print(time.substring(0, spaceIndex1));
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
        myMatrix->print(time.substring(spaceIndex1, dotIndex1));
        myMatrix->setTextColor(myMatrix->Color(0, 40, 0));
        myMatrix->print(time.substring(dotIndex1));
    }


    const String clockTime2 = clockTime.substring(3) + clockTime.substring(0, 3);
    String time2 = clockTime2.substring(indexx);
    if (indexx > 0) {
        time2 += clockTime2.substring(0, indexx);
    }

    myMatrix->setCursor(posx, 8);
    const uint8_t dotIndex2 = time2.indexOf(':');
    const uint8_t spaceIndex2 =  time2.indexOf(' ');
    if (dotIndex2 < spaceIndex2) {
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
        myMatrix->print(time2.substring(0, dotIndex2));
        myMatrix->setTextColor(myMatrix->Color(0, 40, 0));
        myMatrix->print(time2.substring(dotIndex2, spaceIndex2));
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
        myMatrix->print(time2.substring(spaceIndex2));
    } else {
        myMatrix->setTextColor(myMatrix->Color(0, 40, 0));
        myMatrix->print(time2.substring(0, spaceIndex2));
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
        myMatrix->print(time2.substring(spaceIndex2, dotIndex2));
        myMatrix->setTextColor(myMatrix->Color(0, 40, 0));
        myMatrix->print(time2.substring(dotIndex2));
    }


    delay(1);
    myMatrix->show();
}

void ClockHorizontal1Effect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(myMatrix->Color(40, 40, 40));

    if (myMatrix->getRotation() != 0) { // for horizontal only
        myMatrix->setRotation(0);
    }
}

void ClockHorizontal1Effect::deactivate()
{
    GyverTimer::SetInterval(0);
    if (myMatrix->getRotation() != 3) {
        myMatrix->setRotation(3);
    }
}
