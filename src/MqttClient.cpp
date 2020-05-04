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

String commonTopic;

String availabilityTopic;
String configTopic;
String setTopic;
String stateTopic;

String clientId;

void subscribe()
{
    client->subscribe(setTopic.c_str());
}

bool sendJson(const char* topic, const DynamicJsonDocument &doc)
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

    boolean success = sendJson(stateTopic.c_str(), doc);
    Serial.printf_P(PSTR("State sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

void sendAvailability()
{
    Serial.println(F("Sending availability"));
    boolean success = client->publish_P(availabilityTopic.c_str(), PSTR("true"), true);
    Serial.printf_P(PSTR("Availability sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

void sendDiscovery()
{
    DynamicJsonDocument doc(1024*5);
    doc[F("~")] = commonTopic;
    doc[F("name")] = mySettings->connectionSettings.mdns;
    doc[F("uniq_id")] = mySettings->connectionSettings.uniqueId;
    doc[F("cmd_t")] = F("~/set");
    doc[F("stat_t")] = F("~/state");
    doc[F("avty_t")] = F("~/available");
    doc[F("pl_avail")] = F("true");
    doc[F("pl_not_avail")] = F("false");
    doc[F("schema")] = F("json");
    doc[F("brightness")] = true;
    doc[F("effect")] = true;
    doc[F("dev")][F("ids")] = mySettings->connectionSettings.uniqueId;
    doc[F("dev")][F("mf")] = mySettings->connectionSettings.manufacturer;
    doc[F("dev")][F("name")] = mySettings->connectionSettings.mdns;
    doc[F("dev")][F("mdl")] = mySettings->connectionSettings.apName;

    JsonArray effects = doc.createNestedArray(F("effect_list"));
    mySettings->WriteEffectsMqtt(effects);

    Serial.println(F("Sending discovery"));
    serializeJsonPretty(doc, Serial);
    Serial.println();

    boolean success = sendJson(configTopic.c_str(), doc);
    Serial.printf_P(PSTR("Discovery sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

void reconnect()
{
    static bool connecting = false;

    if (!client || client->connected() || connecting) {
        return;
    }

    connecting = true;
    Serial.print(F("Attempting MQTT connection "));
    clientId = String(F("FireLampClient-")) + mySettings->connectionSettings.mdns;
    if (client->connect(clientId.c_str(),
                        mySettings->mqttSettings.username.c_str(),
                        mySettings->mqttSettings.password.c_str(),
                        availabilityTopic.c_str(),
                        1,
                        true,
                        "false")) {
        connecting = false;
        Serial.println(F("succeeded"));

        sendDiscovery();
        sendState();
        sendAvailability();
        subscribe();
    } else {
        connecting = false;
        Serial.println(F("failed"));

#if defined(ESP8266)
        ESP.wdtFeed();
#else
        yield();
#endif
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
    Serial.println();

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

    if (client->connected()) {
        client->loop();
    } else {
        reconnect();
    }
}

void MqttClient::update()
{
    if (!client) {
        return;
    }

    sendState();
}

MqttClient::MqttClient()
{
    if (mySettings->mqttSettings.host.isEmpty()) {
        Serial.println(F("Empty host for MqttClient"));
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
