#pragma once

#define mqtt MqttClient::Instance()

class MqttClient
{
public:
    static MqttClient *Instance();
    static void Initialize();
    void loop();
    void update();

protected:
    MqttClient();
};

