#include "MqttClient.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "Settings.h"

namespace
{

MqttClient *instance = nullptr;

WiFiClient wifiClient;
PubSubClient *client = nullptr;

String availabilityTopic;
String commonTopic;
String configTopic;
String setTopic;
String stateTopic;

String clientId;

void subscribe()
{
    client->subscribe(setTopic.c_str());
}

bool sendJson(const char* topic, JsonDocument doc)
{
    size_t len = measureJson(doc);

    if (!client->beginPublish(topic, len, true)) {
        Serial.println(F("beginPublish failed!"));
        return false;
    }

    if (serializeJson(doc, *client) != len) {
        Serial.println(F("writing payload: wrong size!"));
        return false;
    }

    if (!client->endPublish()) {
        Serial.println(F("endPublish failed!"));
        return false;
    }

    return true;
}

void sendState()
{
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();

    mySettings->BuildJsonMqtt(json);

    Serial.println(F("Sending state"));
    serializeJsonPretty(doc, Serial);
    Serial.println();

    sendJson(stateTopic.c_str(), doc);
}

void sendDiscovery()
{
    DynamicJsonDocument doc(1024*5);
    doc[F("~")] = commonTopic;
    doc[F("name")] = mySettings->connectionSettings.mdns;
    doc[F("unique_id")] = mySettings->connectionSettings.mdns;
    doc[F("cmd_t")] = F("~/set");
    doc[F("stat_t")] = F("~/state");
    doc[F("schema")] = F("json");
    doc[F("brightness")] = true;
//    doc[F("rgb")] = true;
    doc[F("effect")] = true;


    JsonArray effects = doc.createNestedArray(F("effect_list"));
    mySettings->WriteEffectsMqtt(effects);

    Serial.println(F("Sending discovery"));
    serializeJsonPretty(doc, Serial);
    Serial.println();

    sendJson(configTopic.c_str(), doc);
}

void reconnect()
{
    if (client->connected()) {
        return;
    }

    Serial.print(F("Attempting MQTT connection "));
    uint8_t tries = 0;

    while (!client->connected() && tries < 10) {
        clientId = String(F("FireLampClient-")) + String(random(0xffff), HEX);
        // client->setWill(availabilityTopic, String(F("false")), true);
        if (client->connect(clientId.c_str(),
                            mySettings->mqttSettings.username.c_str(),
                            mySettings->mqttSettings.password.c_str(),
                            availabilityTopic.c_str(),
                            0, true, String(F("false")).c_str())) {
            Serial.println(F(" connected"));

            sendDiscovery();
            sendState();
            subscribe();
            client->publish(availabilityTopic.c_str(), String(F("true")).c_str(), true);
        } else {
            Serial.print('.');
            delay(1000);
            ++tries;
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.println(topic);

    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, payload, length) != DeserializationError::Ok) {
        Serial.println(F("Error parsing json data"));
        return;
    }
    JsonObject json = doc.as<JsonObject>();

    serializeJsonPretty(doc, Serial);

    mySettings->ProcessCommandMqtt(json);

    sendState();
}

}

MqttClient *MqttClient::Instance()
{
    return instance;
}

void MqttClient::Initialize()
{
    if (instance) {
        return;
    }

    Serial.println(F("Initializing MqttClient"));
    instance = new MqttClient();
}

void MqttClient::loop()
{
    if (!client) {
        return;
    }

    if (!client->connected()) {
        reconnect();
    }

    client->loop();
}

void MqttClient::update()
{
    sendState();
}

MqttClient::MqttClient()
{
    if (mySettings->mqttSettings.host.isEmpty()) {
        return;
    }

    commonTopic = String(F("homeassistant/light/")) + mySettings->connectionSettings.mdns;
    setTopic = commonTopic + String(F("/set"));
    stateTopic = commonTopic + String(F("/state"));
    configTopic = commonTopic + String(F("/config"));
    availabilityTopic = commonTopic + String(F("/available"));

    client = new PubSubClient(wifiClient);
    client->setServer(mySettings->mqttSettings.host.c_str(),
                      mySettings->mqttSettings.port);
    client->setCallback(callback);
}
