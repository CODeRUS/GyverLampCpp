#pragma once
#include <Arduino.h>

class LocalDNS
{
public:
    static bool Begin();
    static void AddService(String serviceName, String serviceProtocol = "tcp", uint16_t servicePort = 80);
    static void Process();
};

