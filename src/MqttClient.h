#pragma once

#define mqtt MqttClient::instance()

class MqttClient
{
public:
    static MqttClient *instance();
    static void Initialize();
    void update();

protected:
    MqttClient();
};

