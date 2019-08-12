#include "GyverTimer.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

namespace  {

WiFiUDP *ntpUDP = nullptr;
NTPClient *timeClient = nullptr;

uint32_t defaultInterval = 0;
uint32_t interval = 0;
uint32_t timer = 0;

unsigned long hours = 0;
String hoursString = "88";
unsigned long minutes = 0;
String minutesString = "88";
unsigned long seconds = 0;
String secondsString = "88";

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
    if (!timeClient) {
        return;
    }

    if ((millis() - timer) < interval) {
        return;
    }

    timer = millis();
    timeClient->update();

    ReadTime();
}

void GyverTimer::SetInterval(uint32_t timerInterval)
{
    if (timerInterval == 0) {
        interval = defaultInterval;
    } else {
        interval = timerInterval;
    }
    ForceUpdate();
}

void GyverTimer::ForceUpdate()
{
    timeClient->forceUpdate();

    ReadTime();
}

void GyverTimer::ReadTime()
{
    const unsigned long rawTime = timeClient->getEpochTime();
    hours = (rawTime % 86400L) / 3600;
    hoursString = hours < 10 ? "0" + String(hours) : String(hours);

    minutes = (rawTime % 3600) / 60;
    minutesString = minutes < 10 ? "0" + String(minutes) : String(minutes);

    seconds = rawTime % 60;
    secondsString = seconds < 10 ? "0" + String(seconds) : String(seconds);
}

String GyverTimer::Hours()
{
    return hoursString;
}

String GyverTimer::Minutes()
{
    return minutesString;
}

String GyverTimer::Seconds()
{
    return secondsString;
}
