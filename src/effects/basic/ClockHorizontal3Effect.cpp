#include "ClockHorizontal3Effect.h"
#include "GyverTimer.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

} // namespace

ClockHorizontal3Effect::ClockHorizontal3Effect()
{
    effectName = "Clock horizontal single";

    settings = new Settings::EffectSettings();
    settings->effectScale = 1;
    settings->effectSpeed = 250;
    settings->effectBrightness = 10;
}

void ClockHorizontal3Effect::tick()
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
    myMatrix->setCursor(posx, 4);
    myMatrix->print(time);

    delay(1);
    myMatrix->show();
}

void ClockHorizontal3Effect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(myMatrix->Color(40, 40, 40));

    if (myMatrix->getRotation() != 0) { // for horizontal only
        myMatrix->setRotation(0);
    }
}

void ClockHorizontal3Effect::deactivate()
{
    GyverTimer::SetInterval(0);
    if (myMatrix->getRotation() != 3) {
        myMatrix->setRotation(3);
    }
}
