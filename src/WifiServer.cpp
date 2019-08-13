#include "WifiServer.h"
#include <WiFiManager.h>

namespace {

WiFiManager *wifiManager = nullptr;
bool wifiConnected = false;

} // namespace

void WifiServer::Initialize(const char *firstSetupName)
{
    if (wifiManager) {
        delete wifiManager;
    }

    wifiManager = new WiFiManager;
    wifiManager->setConfigPortalBlocking(false);
    wifiConnected = wifiManager->autoConnect(firstSetupName);
    if (wifiConnected) {
        Serial.println("Wifi connected to saved AP!");
    }

    Serial.print("Local ip: ");
    Serial.println(WiFi.localIP());
}

void WifiServer::StartPortal(const char *onDemandName, const char *onDemandPassword)
{
    Serial.println("Non blocking config portal running!");
    wifiManager->startConfigPortal(onDemandName, onDemandPassword);
}

void WifiServer::Process()
{
    wifiManager->process();
}

bool WifiServer::isConnected()
{
    return wifiConnected;
}
