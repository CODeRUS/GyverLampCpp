#include "LocalDNS.h"

#if defined(ESP8266)
#include <ESP8266mDNS.h>
#else
#include <ESPmDNS.h>
#endif

#include "Settings.h"

namespace  {

const char *localHostname PROGMEM = "firelamp";

bool started = false;

} // namespace

bool LocalDNS::Begin()
{
    const char* hostname = mySettings->GetCharField(F("connection"), F("mdns"), localHostname);
    started = MDNS.begin(hostname);
    if (started) {
        Serial.println(F("mDNS responder started!"));
    }
    return started;
}

void LocalDNS::AddService(const char *serviceName, const char *serviceProtocol, uint16_t servicePort)
{
    if (!started) {
        Serial.println(F("Trying to call LocalDNS::AddService, but MDNS is not started!"));
        return;
    }
    Serial.printf_P(PSTR("Announcing %s (%s) service on port %u\n"), serviceName, serviceProtocol, servicePort);
    MDNS.addService(serviceName, serviceProtocol, servicePort);
}
