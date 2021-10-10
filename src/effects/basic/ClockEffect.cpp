#include "ClockEffect.h"
#include "TimeClient.h"

namespace  {

uint8_t delayer = 0;
bool hoursNum = true;

uint16_t hoursColor = myMatrix->Color(40, 40, 40);
uint16_t minutesColor = myMatrix->Color(30, 60, 30);

String getClockTime()
{
    return timeClient->hours() + F(":") + timeClient->minutes();
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

ClockEffect::ClockEffect(const String &id)
    : Effect(id)
{
}

void ClockEffect::tick()
{
    if (delayer < 7) {
        ++delayer;
        return;
    }

    myMatrix->clear();
    delay(1);

    if (hoursNum) {
        myMatrix->setTextColor(hoursColor);
        myMatrix->setCursor(0, 5);
    } else {
        myMatrix->setTextColor(minutesColor);
        myMatrix->setCursor(-13, 5);
    }
    myMatrix->print(getClockTime());
    myMatrix->show();
    delay(10);

    hoursNum = !hoursNum;
    delayer = 0;
}

void ClockEffect::activate()
{
    timeClient->setInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(hoursColor);
}

void ClockEffect::deactivate()
{
}

void ClockEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    readColor16(json, F("hColor"), hoursColor);
    readColor16(json, F("mColor"), minutesColor);
}

void ClockEffect::writeSettings(JsonObject &json)
{
    writeColor16(json, F("hColor"), hoursColor);
    writeColor16(json, F("mColor"), minutesColor);
}
