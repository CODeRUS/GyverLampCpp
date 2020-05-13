#pragma once
#include <Arduino.h>
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>

#define mqtt MqttClient::Instance()

class MqttClient
{
public:
    static MqttClient *Instance();
    static void Initialize();

    void update();

protected:
    MqttClient();
};

