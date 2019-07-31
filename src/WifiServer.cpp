#include "WifiServer.h"
#include <WiFiManager.h>

namespace {

WiFiManager *wifiManager = nullptr;
bool wifiConnected = false;

} // namespace

void WifiServer::Initialize(const char *firstSetupName, const char *onDemandName, const char *onDemandPassword)
{
    if (wifiManager) {
        delete wifiManager;
    }

    wifiManager = new WiFiManager;
    wifiManager->setConfigPortalBlocking(false);
    wifiConnected = wifiManager->autoConnect(firstSetupName);
    if (wifiConnected) {
        Serial.println("Wifi connected to saved AP!");
    } else {
        Serial.println("Non blocking config portal running!");
        wifiManager->startConfigPortal(onDemandName, onDemandPassword);
    }

    Serial.print("Local ip: ");
    Serial.println(WiFi.localIP());
}

void WifiServer::Process()
{
    wifiManager->process();
}

bool WifiServer::isConnected()
{
    return wifiConnected;
}
