#include "LocalDNS.h"

#if defined(ESP8266)
#include <ESP8266mDNS.h>
#else
#include <ESPmDNS.h>
#endif

namespace  {

bool started = false;

} // namespace

bool LocalDNS::Begin(const char *hostname)
{
    started = MDNS.begin(hostname);
    if (started) {
        Serial.println("mDNS responder started!");
    }
    return started;
}

void LocalDNS::AddService(const char *serviceName, const char *serviceProtocol, uint16_t servicePort)
{
    if (!started) {
        Serial.println("Trying to call LocalDNS::AddService, but MDNS is not started!");
        return;
    }
    MDNS.addService(serviceName, serviceProtocol, servicePort);
}
