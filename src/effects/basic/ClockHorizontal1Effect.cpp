#include "ClockHorizontal1Effect.h"
#include "TimeClient.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

uint16_t hoursColor = myMatrix->Color(40, 40, 40);
uint16_t minutesColor = myMatrix->Color(30, 60, 30);

String getClockTime()
{
    return timeClient->hours() + ":" + timeClient->minutes() + " ";
}

} // namespace

ClockHorizontal1Effect::ClockHorizontal1Effect()
{
}

void ClockHorizontal1Effect::tick()
{
    myMatrix->clear();
    delay(1);

    const String clockTime = getClockTime();
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

    myMatrix->setCursor(posx, 0);
    const uint8_t dotIndex1 = time.indexOf(':');
    const uint8_t spaceIndex1 = time.indexOf(' ');
    if (dotIndex1 < spaceIndex1) {
        myMatrix->setTextColor(hoursColor);
        myMatrix->print(time.substring(0, dotIndex1));
        myMatrix->setTextColor(minutesColor);
        myMatrix->print(time.substring(dotIndex1, spaceIndex1));
        myMatrix->setTextColor(hoursColor);
        myMatrix->print(time.substring(spaceIndex1));
    } else {
        myMatrix->setTextColor(minutesColor);
        myMatrix->print(time.substring(0, spaceIndex1));
        myMatrix->setTextColor(hoursColor);
        myMatrix->print(time.substring(spaceIndex1, dotIndex1));
        myMatrix->setTextColor(minutesColor);
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
        myMatrix->setTextColor(hoursColor);
        myMatrix->print(time2.substring(0, dotIndex2));
        myMatrix->setTextColor(minutesColor);
        myMatrix->print(time2.substring(dotIndex2, spaceIndex2));
        myMatrix->setTextColor(hoursColor);
        myMatrix->print(time2.substring(spaceIndex2));
    } else {
        myMatrix->setTextColor(minutesColor);
        myMatrix->print(time2.substring(0, spaceIndex2));
        myMatrix->setTextColor(hoursColor);
        myMatrix->print(time2.substring(spaceIndex2, dotIndex2));
        myMatrix->setTextColor(minutesColor);
        myMatrix->print(time2.substring(dotIndex2));
    }

    myMatrix->show();
    delay(10);
}

void ClockHorizontal1Effect::activate()
{
    timeClient->setInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(hoursColor);

    uint8_t matrixRotation = myMatrix->getRotation();
    int horizontalRotation = matrixRotation - 3;
    if (horizontalRotation < 0) {
        horizontalRotation = horizontalRotation + 4;
    }

    if (myMatrix->getRotation() != horizontalRotation) { // for horizontal only
        myMatrix->setRotation(horizontalRotation);
    }
}

void ClockHorizontal1Effect::deactivate()
{
    timeClient->setInterval(0);
    uint8_t matrixRotation = myMatrix->getRotation();
    if (myMatrix->getRotation() != matrixRotation) {
        myMatrix->setRotation(matrixRotation);
    }
}

void ClockHorizontal1Effect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("hColor"))) {
        hoursColor = json[F("hColor")];
    }
    if (json.containsKey(F("mColor"))) {
        minutesColor = json[F("mColor")];
    }
}

void ClockHorizontal1Effect::writeSettings(JsonObject &json)
{
    json[F("hColor")] = hoursColor;
    json[F("mColor")] = minutesColor;
}
