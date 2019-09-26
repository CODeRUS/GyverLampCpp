#include "LocalDNS.h"

#if defined(ESP8266)
#include <ESP8266mDNS.h>
#else
#include <ESPmDNS.h>
#endif

#include "Settings.h"

namespace  {

bool started = false;

} // namespace

bool LocalDNS::Begin()
{
    started = MDNS.begin(mySettings->connectionSettings.mdns);
    if (started) {
        Serial.printf_P(PSTR("mDNS responder (%s) started!\n"), mySettings->connectionSettings.mdns.c_str());
    }
    return started;
}

void LocalDNS::AddService(String serviceName, String serviceProtocol, uint16_t servicePort)
{
    if (!started) {
        Serial.println(F("Trying to call LocalDNS::AddService, but MDNS is not started!"));
        return;
    }
    Serial.printf_P(PSTR("Announcing %s (%s) service on port %u\n"), serviceName.c_str(), serviceProtocol.c_str(), servicePort);
    MDNS.addService(serviceName, serviceProtocol, servicePort);
}

void LocalDNS::Process()
{
#if defined(ESP8266)
    MDNS.update();
#endif
}
