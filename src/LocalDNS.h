#pragma once
#include <Arduino.h>

class LocalDNS
{
public:
    static bool Begin(const char* hostname);
    static void AddService(const char* serviceName, const char* serviceProtocol = "tcp", uint16_t servicePort = 80);
};

