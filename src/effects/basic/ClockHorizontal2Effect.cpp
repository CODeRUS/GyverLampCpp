#include "ClockHorizontal2Effect.h"
#include "TimeClient.h"

namespace  {

int8_t posx = 0;
uint8_t indexx = 0;

uint16_t hoursColor = myMatrix->Color(40, 40, 40);
uint16_t minutesColor = myMatrix->Color(30, 60, 30);

String getClockTime1()
{
    return timeClient->hours() + ":" + timeClient->hours() + ":";
}

String getClockTime2()
{
    return timeClient->minutes() + " " + timeClient->minutes() + " ";
}

void readColor16(const JsonObject &json, const String &key, uint16_t &myColor)
{
    if (json.containsKey(key)) {
        const JsonVariant color = json[key];
        if (color.is<uint16_t>()) {
            myColor = json[key];
        } else if (color.is<JsonObject>()) {
            uint32_t color = json[key]["r"].as<uint8_t>() << 16 |
                             json[key]["g"].as<uint8_t>() << 8 |
                             json[key]["b"].as<uint8_t>();
            myColor = myMatrix->Color24to16(color);
        }
    }
}

void writeColor16(JsonObject &json, const String &key, uint16_t myColor)
{
    uint32_t color32 = myMatrix->expandColor(myColor);
    JsonObject color = json.createNestedObject(key);
    color["r"] = (uint8_t)(color32 >> 16);
    color["g"] = (uint8_t)(color32 >> 8);
    color["b"] = (uint8_t)(color32);
}

} // namespace

ClockHorizontal2Effect::ClockHorizontal2Effect(const String &id)
    : Effect(id)
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
    timeClient->setInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);

    uint8_t matrixRotation = mySettings->matrixSettings.rotation;
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
    timeClient->setInterval(0);
    uint8_t matrixRotation = myMatrix->getRotation();
    uint8_t settingsRotation = mySettings->matrixSettings.rotation;
    if (matrixRotation != settingsRotation) {
        myMatrix->setRotation(settingsRotation);
    }
}

void ClockHorizontal2Effect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    readColor16(json, F("hColor"), hoursColor);
    readColor16(json, F("mColor"), minutesColor);
}

void ClockHorizontal2Effect::writeSettings(JsonObject &json)
{
    writeColor16(json, F("hColor"), hoursColor);
    writeColor16(json, F("mColor"), minutesColor);
}
