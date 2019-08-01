#include "GyverTimer.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

namespace  {

WiFiUDP *ntpUDP = nullptr;
NTPClient *timeClient = nullptr;

uint32_t defaultInterval = 0;
uint32_t interval = 0;
uint32_t timer = 0;

String formattedTime = "88:88:88";
String clockTime = "88:88";

} // namespace

void GyverTimer::Initialize(const char* poolServerName, int timeOffset, int updateInterval, uint32_t timerInterval)
{
    ntpUDP = new WiFiUDP;
    timeClient = new NTPClient(*ntpUDP, poolServerName, timeOffset, updateInterval);
    timeClient->begin();

    defaultInterval = timerInterval;
    interval = timerInterval;
    timer = millis();
}

void GyverTimer::Process()
{
    if ((millis() - timer) < interval) {
        return;
    }

    timer = millis();
    timeClient->update();

    const unsigned long rawTime = timeClient->getEpochTime();
    const unsigned long hours = (rawTime % 86400L) / 3600;
    const String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

    const unsigned long minutes = (rawTime % 3600) / 60;
    const String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);
    clockTime = hoursStr + ":" + minuteStr;

    const unsigned long seconds = rawTime % 60;
    const String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

    formattedTime = hoursStr + ":" + minuteStr + ":" + secondStr;
}

void GyverTimer::SetInterval(uint32_t timerInterval)
{
    if (timerInterval == 0) {
        interval = defaultInterval;
    } else {
        interval = timerInterval;
    }
    Process();
}

void GyverTimer::ForceUpdate()
{
    timeClient->forceUpdate();
}

String GyverTimer::FormattedTime()
{
    return formattedTime;
}

String GyverTimer::ClockTime()
{
    if (clockTime == "88:88") {
        ForceUpdate();
    }
    return clockTime;
}
