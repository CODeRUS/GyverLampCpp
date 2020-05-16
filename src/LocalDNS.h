#pragma once
#include <Arduino.h>

#define localDNS LocalDNS::instance()

class LocalDNS
{
public:
    static LocalDNS *instance();
    static void Initialize();

    bool begin();
    void addService(String serviceName, String serviceProtocol = "tcp", uint16_t servicePort = 80);
    void loop();

protected:
    LocalDNS();
};

