#pragma once
#include <Arduino.h>

#define timeClient TimeClient::instance()

class TimeClient
{
public:
    static void Initialize();
    static TimeClient *instance();

    void loop();
    void setInterval(uint32_t timerInterval);
    void forceUpdate();
    void readTime();
    String hours();
    String minutes();
    String seconds();

protected:
    TimeClient();
};

