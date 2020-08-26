#include "LocalDNS.h"

#if defined(ESP8266)
#include <ESP8266mDNS.h>
#else
#include <ESPmDNS.h>
#endif

#include "Settings.h"

namespace  {

LocalDNS *object = nullptr;

bool started = false;

} // namespace

bool LocalDNS::begin()
{
    if (mySettings->connectionSettings.mdns.isEmpty()) {
        return false;
    }

    started = MDNS.begin(mySettings->connectionSettings.mdns.c_str());
    if (started) {
        Serial.printf_P(PSTR("mDNS responder (%s) started!\n"), mySettings->connectionSettings.mdns.c_str());
    }
    return started;
}

void LocalDNS::addService(String serviceName, String serviceProtocol, uint16_t servicePort)
{
    if (!started) {
        Serial.println(F("Trying to call LocalDNS::AddService, but MDNS is not started!"));
        return;
    }
    Serial.printf_P(PSTR("Announcing %s (%s) service on port %u\n"), serviceName.c_str(), serviceProtocol.c_str(), servicePort);
    MDNS.addService(serviceName, serviceProtocol, servicePort);
}

void LocalDNS::loop()
{
#if defined(ESP8266)
    if (started) {
        MDNS.update();
    }
#endif
}

LocalDNS::LocalDNS()
{

}

LocalDNS *LocalDNS::instance()
{
    return object;
}

void LocalDNS::Initialize()
{
    if (object) {
        return;
    }

    Serial.println(F("Initializing LocalDNS"));
    object = new LocalDNS;
}
