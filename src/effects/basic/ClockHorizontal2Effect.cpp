#include "ClockHorizontal2Effect.h"
#include "GyverTimer.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

uint16_t hoursColor = myMatrix->Color(40, 40, 40);
uint16_t minutesColor = myMatrix->Color(30, 60, 30);

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
}

void ClockHorizontal2Effect::tick()
{
    myMatrix->clear();
    delay(1);

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

    myMatrix->setTextColor(hoursColor);
    myMatrix->setCursor(posx, 0);
    myMatrix->print(time);

    const String clockTime2 = getClockTime2();
    String time2 = clockTime2.substring(indexx);
    if (indexx > 0) {
        time2 += clockTime2.substring(0, indexx);
    }

    myMatrix->setCursor(posx, 8);
    myMatrix->setTextColor(minutesColor);
    myMatrix->print(time2);

    myMatrix->show();
    delay(10);
}

void ClockHorizontal2Effect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);

    uint8_t matrixRotation = myMatrix->GetRotation();
    int horizontalRotation = matrixRotation - 3;
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
    uint8_t matrixRotation = myMatrix->GetRotation();
    if (myMatrix->getRotation() != matrixRotation) {
        myMatrix->setRotation(matrixRotation);
    }
}

void ClockHorizontal2Effect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("hColor"))) {
        hoursColor = json[F("hColor")];
    }
    if (json.containsKey(F("mColor"))) {
        minutesColor = json[F("mColor")];
    }
}

void ClockHorizontal2Effect::writeSettings(JsonObject &json)
{
    json[F("hColor")] = hoursColor;
    json[F("mColor")] = minutesColor;
}
