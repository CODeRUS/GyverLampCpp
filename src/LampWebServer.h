#pragma once
#include <Arduino.h>

#define lampWebServer LampWebServer::Instance()

class LampWebServer
{
public:
    static LampWebServer *Instance();
    static void Initialize(uint16_t webPort = 80, uint16_t wsPort = 8000);
    bool IsConnected();
    void AutoConnect(const char *apName);
    void StartServer();
    void Process();
    void SendConfig();
    bool isUpdating();

private:
    void configureHandlers();

protected:
    LampWebServer(uint16_t webPort, uint16_t wsPort);
};

