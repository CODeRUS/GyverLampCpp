#include "ClockHorizontal3Effect.h"
#include "TimeClient.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

uint16_t hoursColor = myMatrix->Color(40, 40, 40);
uint16_t minutesColor = myMatrix->Color(30, 60, 30);

String getClockTime()
{
    return timeClient->hours() + timeClient->minutes();
}

bool colored = true;
void swapMatrixColor()
{
    if (colored) {
        myMatrix->setTextColor(hoursColor);
    } else {
        myMatrix->setTextColor(minutesColor);
    }
    colored = !colored;
}

} // namespace

ClockHorizontal3Effect::ClockHorizontal3Effect(const String &id)
    : Effect(id)
{
}

void ClockHorizontal3Effect::tick()
{
    myMatrix->clear();
    delay(1);

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

    myMatrix->setCursor(posx, 4);
    for (uint8_t index = 0; index < time.length(); index++) {
        if (index % 2 == halfIndex) {
            swapMatrixColor();
        }
        myMatrix->print(time[index]);
    }

    myMatrix->show();
    delay(10);
}

void ClockHorizontal3Effect::activate()
{
    timeClient->setInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(hoursColor);

    uint8_t matrixRotation = mySettings->matrixSettings.rotation;
    int horizontalRotation = matrixRotation - 3;
    if (horizontalRotation < 0) {
        horizontalRotation = horizontalRotation + 4;
    }

    if (myMatrix->getRotation() != horizontalRotation) { // for horizontal only
        myMatrix->setRotation(horizontalRotation);
    }
}

void ClockHorizontal3Effect::deactivate()
{
    timeClient->setInterval(0);
    uint8_t matrixRotation = myMatrix->getRotation();
    uint8_t settingsRotation = mySettings->matrixSettings.rotation;
    if (matrixRotation != settingsRotation) {
        myMatrix->setRotation(settingsRotation);
    }
}

void ClockHorizontal3Effect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("hColor"))) {
        hoursColor = json[F("hColor")];
    }
    if (json.containsKey(F("mColor"))) {
        minutesColor = json[F("mColor")];
    }
}

void ClockHorizontal3Effect::writeSettings(JsonObject &json)
{
    json[F("hColor")] = hoursColor;
    json[F("mColor")] = minutesColor;
}
