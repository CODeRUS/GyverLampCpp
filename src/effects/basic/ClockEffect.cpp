#include "ClockEffect.h"
#include "GyverTimer.h"

namespace  {

uint8_t delayer = 0;
bool hours = true;

uint16_t hoursColor = myMatrix->Color(40, 40, 40);
uint16_t minutesColor = myMatrix->Color(30, 60, 30);

String getClockTime()
{
    return GyverTimer::Hours() + PSTR(":") + GyverTimer::Minutes();
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
        myMatrix->setCursor(0, 0);
    } else {
        myMatrix->setTextColor(minutesColor);
        myMatrix->setCursor(-13, 0);
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
    if (json.containsKey(F("hoursColor"))) {
        hoursColor = json[F("hoursColor")];
    }
    if (json.containsKey(F("minutesColor"))) {
        minutesColor = json[F("minutesColor")];
    }
}

void ClockEffect::writeSettings(JsonObject &json)
{
    json[F("hoursColor")] = hoursColor;
    json[F("minutesColor")] = minutesColor;
}
