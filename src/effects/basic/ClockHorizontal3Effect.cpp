#include "ClockHorizontal3Effect.h"
#include "GyverTimer.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

String getClockTime()
{
    return GyverTimer::Hours() + GyverTimer::Minutes();
}

bool hoursColor = true;
void swapMatrixColor()
{
    if (hoursColor) {
        myMatrix->setTextColor(myMatrix->Color(40, 40, 40));
    } else {
        myMatrix->setTextColor(myMatrix->Color(30, 60, 30));
    }
    hoursColor = !hoursColor;
}

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

    const String clockTime = getClockTime();
    if (--posx == -6) {
        posx = 0;
        if (indexx % 2 == 0) {
            swapMatrixColor();
        }
        if (++indexx == clockTime.length()) {
            indexx = 0;
        }
    }
    const uint8_t halfIndex = indexx % 2;

    String time = clockTime.substring(indexx);
    if (indexx > 0) {
        time += clockTime.substring(0, indexx);
    }

//    swapMatrixColor();
    myMatrix->setCursor(posx, 4);
    for (uint8_t index = 0; index < time.length(); index++) {
        if (index % 2 == halfIndex) {
            swapMatrixColor();
        }
        myMatrix->print(time[index]);
    }

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
