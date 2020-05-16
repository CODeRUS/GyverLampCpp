#pragma once
#include <Arduino.h>

#define lampWebServer LampWebServer::instance()

class AsyncWebSocket;
class AsyncWebSocketClient;
class LampWebServer
{
public:
    static LampWebServer *instance();
    static void Initialize(uint16_t webPort = 80);

    bool isConnected();
    void autoConnect();
    void loop();
    void sendConfig();
    bool isUpdating();
    void onConnected(void (*func)(bool));
    void update();

protected:
    LampWebServer(uint16_t webPort);
};

