#pragma once

#define mqtt MqttClient::instance()

class MqttClient
{
public:
    static MqttClient *instance();
    static void Initialize();

    void loop();
    void update();

protected:
    MqttClient();
};

