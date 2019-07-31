#include "GyverTimer.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

namespace  {

WiFiUDP *ntpUDP = nullptr;
NTPClient *timeClient = nullptr;

uint32_t interval = 0;
uint32_t timer = 0;

} // namespace

void GyverTimer::Initialize(const char* poolServerName, int timeOffset, int updateInterval, uint32_t timerInterval)
{
    ntpUDP = new WiFiUDP;
    timeClient = new NTPClient(*ntpUDP, poolServerName, timeOffset, updateInterval);
    timeClient->begin();

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
}

String GyverTimer::ToString()
{
    return timeClient->getFormattedTime();
}
