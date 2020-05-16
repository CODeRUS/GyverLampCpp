#include "TimeClient.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "Settings.h"

namespace  {

TimeClient *object = nullptr;

int timeOffset = 3 * 3600; // GMT + 3
int updateInterval = 60 * 1000; // 1 min

WiFiUDP *wifiUdp = nullptr;
NTPClient *ntp = nullptr;

uint32_t defaultInterval = 5 * 60 * 1000; // 5 min
uint32_t interval = 0;
uint32_t timer = 0;

unsigned long hoursNum = 0;
String hoursString = "88";
unsigned long minutesNum = 0;
String minutesString = "88";
unsigned long secondsNum = 0;
String secondsString = "88";

} // namespace

void TimeClient::Initialize()
{
    if (object) {
        return;
    }

    Serial.println(F("Initializing TimeClient"));
    object = new TimeClient;
}

TimeClient *TimeClient::instance()
{
    return object;
}

void TimeClient::loop()
{
    if (!ntp) {
        return;
    }

    if ((millis() - timer) < interval) {
        return;
    }

    timer = millis();
    ntp->update();

    readTime();
}

void TimeClient::setInterval(uint32_t timerInterval)
{
    if (!ntp) {
        return;
    }

    if (timerInterval == 0) {
        interval = defaultInterval;
    } else {
        interval = timerInterval;
    }
    forceUpdate();
}

void TimeClient::forceUpdate()
{
    if (!ntp) {
        return;
    }

    ntp->forceUpdate();

    readTime();
}

void TimeClient::readTime()
{
    if (!ntp) {
        return;
    }

    const unsigned long rawTime = ntp->getEpochTime();
    hoursNum = (rawTime % 86400L) / 3600;
    hoursString = hoursNum < 10 ? String(0) + String(hoursNum) : String(hoursNum);

    minutesNum = (rawTime % 3600) / 60;
    minutesString = minutesNum < 10 ? String(0) + String(minutesNum) : String(minutesNum);

    secondsNum = rawTime % 60;
    secondsString = secondsNum < 10 ? String(0) + String(secondsNum) : String(secondsNum);
}

String TimeClient::hours()
{
    return hoursString;
}

String TimeClient::minutes()
{
    return minutesString;
}

String TimeClient::seconds()
{
    return secondsString;
}

TimeClient::TimeClient()
{
    wifiUdp = new WiFiUDP;

    ntp = new NTPClient(
        *wifiUdp,
        mySettings->connectionSettings.ntpServer.c_str(),
        mySettings->connectionSettings.ntpOffset,
        updateInterval);
    ntp->begin();

    interval = defaultInterval;
    timer = millis();

    Serial.printf_P(
        PSTR("Initializing GyverTimer: %s, offset: %d\n"),
        mySettings->connectionSettings.ntpServer.c_str(),
                timeOffset);
}
