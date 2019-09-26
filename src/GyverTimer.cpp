#include "GyverTimer.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "Settings.h"

namespace  {

int timeOffset = 3 * 3600; // GMT + 3
int updateInterval = 60 * 1000; // 1 min

WiFiUDP *ntpUDP = nullptr;
NTPClient *timeClient = nullptr;

uint32_t defaultInterval = 5 * 60 * 1000; // 5 min
uint32_t interval = 0;
uint32_t timer = 0;

unsigned long hours = 0;
String hoursString = "88";
unsigned long minutes = 0;
String minutesString = "88";
unsigned long seconds = 0;
String secondsString = "88";

} // namespace

void GyverTimer::Initialize()
{
    ntpUDP = new WiFiUDP;

    timeClient = new NTPClient(*ntpUDP,
                               mySettings->connectionSettings.ntpServer.c_str(),
                               mySettings->connectionSettings.ntpOffset,
                               updateInterval);
    timeClient->begin();

    interval = defaultInterval;
    timer = millis();

    Serial.printf_P(PSTR("Initializing GyverTimer: %s, offset: %d\n"),
                    mySettings->connectionSettings.ntpServer.c_str(),
                    timeOffset);
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
    if (!timeClient) {
        return;
    }

    if (timerInterval == 0) {
        interval = defaultInterval;
    } else {
        interval = timerInterval;
    }
    ForceUpdate();
}

void GyverTimer::ForceUpdate()
{
    if (!timeClient) {
        return;
    }

    timeClient->forceUpdate();

    ReadTime();
}

void GyverTimer::ReadTime()
{
    if (!timeClient) {
        return;
    }

    const unsigned long rawTime = timeClient->getEpochTime();
    hours = (rawTime % 86400L) / 3600;
    hoursString = hours < 10 ? PSTR("0") + String(hours) : String(hours);

    minutes = (rawTime % 3600) / 60;
    minutesString = minutes < 10 ? PSTR("0") + String(minutes) : String(minutes);

    seconds = rawTime % 60;
    secondsString = seconds < 10 ? PSTR("0") + String(seconds) : String(seconds);
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
