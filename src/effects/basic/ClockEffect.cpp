#include "ClockEffect.h"
#include "GyverTimer.h"

namespace  {

uint8_t delayer = 0;
bool hours = true;

uint16_t hoursColor = myMatrix->Color(40, 40, 40);
uint16_t minutesColor = myMatrix->Color(30, 60, 30);

String getClockTime()
{
    return GyverTimer::Hours() + F(":") + GyverTimer::Minutes();
}

} // namespace

ClockEffect::ClockEffect()
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

    if (hours) {
        myMatrix->setTextColor(hoursColor);
        myMatrix->setCursor(0, 5);
    } else {
        myMatrix->setTextColor(minutesColor);
        myMatrix->setCursor(-13, 5);
    }
    myMatrix->print(getClockTime());
    myMatrix->show();
    delay(10);

    hours = !hours;
    delayer = 0;
}

void ClockEffect::activate()
{
    GyverTimer::SetInterval(1 * 60 * 1000); // 1 min

    myMatrix->setTextWrap(false);
    myMatrix->setTextColor(hoursColor);
}

void ClockEffect::deactivate()
{
}

void ClockEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("hColor"))) {
        hoursColor = json[F("hColor")];
    }
    if (json.containsKey(F("mColor"))) {
        minutesColor = json[F("mColor")];
    }
}

void ClockEffect::writeSettings(JsonObject &json)
{
    json[F("hColor")] = hoursColor;
    json[F("mColor")] = minutesColor;
}
