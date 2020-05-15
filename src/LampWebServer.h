#pragma once
#include <Arduino.h>

#define lampWebServer LampWebServer::Instance()

class AsyncWebSocket;
class AsyncWebSocketClient;
class LampWebServer
{
public:
    static LampWebServer *Instance();
    static void Initialize(uint16_t webPort = 80);
    bool IsConnected();
    void AutoConnect();
    void Process();
    void SendConfig();
    bool isUpdating();
    void onConnected(void (*func)(bool));
    void Update();

private:
    void configureHandlers();

protected:
    LampWebServer(uint16_t webPort);
};

