#include "MqttClient.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <Ticker.h>

#include "Settings.h"

namespace
{

MqttClient *instance = nullptr;

WiFiClient wifiClient;
AsyncMqttClient *client = nullptr;

String commonTopic;

String availabilityTopic;
String configTopic;
String setTopic;
String stateTopic;

String clientId;

Ticker wifiReconnectTimer;
Ticker mqttReconnectTimer;

void subscribe()
{
    client->subscribe(setTopic.c_str(), 2);
}

bool sendJson(const char* topic, const DynamicJsonDocument &doc)
{
    String buffer;
    if (!serializeJson(doc, buffer)) {
        Serial.println(F("writing payload: wrong size!"));
        return false;
    }
    client->publish(topic, 2, false, buffer.c_str(), buffer.length());

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
    boolean success = client->publish(availabilityTopic.c_str(), 2, true, "true", 4);
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

void connectToMqtt() {
    Serial.println(F("Connecting to MQTT..."));
    client->connect();
}

//void callback(char* topic, byte* payload, unsigned int length)
void callback(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t length, size_t index, size_t total)
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

void onMqttConnect(bool sessionPresent)
{
    Serial.println(F("Connected to MQTT."));
    Serial.print(F("Session present: "));
    Serial.println(sessionPresent);

    sendDiscovery();
    sendState();
    sendAvailability();
    subscribe();
}


void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.print(F("MQTT disconnect reason: "));
    switch (reason) {
    case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
        Serial.println(F("TCP_DISCONNECTED"));
        break;
    case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
        Serial.println(F("MQTT_UNACCEPTABLE_PROTOCOL_VERSION"));
        break;
    case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
        Serial.println(F("MQTT_IDENTIFIER_REJECTED"));
        break;
    case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
        Serial.println(F("MQTT_SERVER_UNAVAILABLE"));
        break;
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
        Serial.println(F("MQTT_MALFORMED_CREDENTIALS"));
        break;
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
        Serial.println(F("MQTT_NOT_AUTHORIZED"));
        break;
    case AsyncMqttClientDisconnectReason::ESP8266_NOT_ENOUGH_SPACE:
        Serial.println(F("ESP8266_NOT_ENOUGH_SPACE"));
        break;
    case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:
        Serial.println(F("TLS_BAD_FINGERPRINT"));
        break;
    default:
        Serial.printf_P(PSTR("unknown %d\n"), reason);
    }

    if (WiFi.isConnected()) {
        mqttReconnectTimer.once(2, connectToMqtt);
    }
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

    client = new AsyncMqttClient;
    client->setServer(mySettings->mqttSettings.host.c_str(),
                      mySettings->mqttSettings.port);
    client->onConnect(onMqttConnect);
    client->onDisconnect(onMqttDisconnect);
    client->onMessage(callback);

    clientId = String(F("FireLampClient-")) + mySettings->connectionSettings.mdns;
    client->setClientId(clientId.c_str());
    client->setWill(availabilityTopic.c_str(),
                    1,
                    true,
                    "false",
                    5);
    client->setCredentials(mySettings->mqttSettings.username.c_str(),
                           mySettings->mqttSettings.password.c_str());
    connectToMqtt();
}
