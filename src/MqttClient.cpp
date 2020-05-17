#include "MqttClient.h"

#include <Arduino.h>
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#else
#include <WiFi.h>
#include <PubSubClient.h>
#endif

#include "Settings.h"

namespace
{

MqttClient *object = nullptr;
#if defined(ESP8266)
AsyncMqttClient *client = nullptr;
#else
unsigned long lastReconnectAttempt = 0;
PubSubClient *client = nullptr;
#endif

String commonTopic;

String availabilityTopic;
String configTopic;
String setTopic;
String stateTopic;

String clientId;

void subscribe()
{
    Serial.print(F("Subscribing to topic: "));
    Serial.println(setTopic);
    client->subscribe(setTopic.c_str(), 0);
}

bool sendJson(const char* topic, const DynamicJsonDocument &doc)
{
#if defined(ESP8266)
    String buffer;
    if (!serializeJson(doc, buffer)) {
        Serial.println(F("writing payload: wrong size!"));
        return false;
    }
    client->publish(topic, 2, false, buffer.c_str(), buffer.length());
#else
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
#endif
    return true;
}

void sendState()
{
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();

    mySettings->buildJsonMqtt(json);

    Serial.println(F("Sending state"));
    Serial.println(stateTopic);
    serializeJsonPretty(doc, Serial);
    Serial.println();

    boolean success = sendJson(stateTopic.c_str(), doc);
    Serial.printf_P(PSTR("State sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

void sendAvailability()
{
    Serial.println(F("Sending availability"));
    Serial.println(availabilityTopic);
#if defined(ESP8266)
    boolean success = client->publish(availabilityTopic.c_str(), 2, true, "true", 4);
#else
    boolean success = client->publish_P(availabilityTopic.c_str(), PSTR("true"), true);
#endif
    Serial.printf_P(PSTR("Availability sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

void sendDiscovery()
{
    DynamicJsonDocument doc(1024*5);
    doc[F("~")] = commonTopic;
    doc[F("name")] = mySettings->mqttSettings.name;
    doc[F("uniq_id")] = mySettings->mqttSettings.uniqueId;
    doc[F("cmd_t")] = F("~/set");
    doc[F("stat_t")] = F("~/state");
    doc[F("avty_t")] = F("~/available");
    doc[F("pl_avail")] = F("true");
    doc[F("pl_not_avail")] = F("false");
    doc[F("schema")] = F("json");
    doc[F("brightness")] = true;
    doc[F("effect")] = true;

    JsonObject dev = doc.createNestedObject(F("dev"));
    dev[F("mf")] = mySettings->mqttSettings.manufacturer;
    dev[F("name")] = mySettings->mqttSettings.name;
    dev[F("mdl")] = mySettings->mqttSettings.model;
    JsonArray ids = dev.createNestedArray(F("ids"));
    ids.add(mySettings->mqttSettings.uniqueId);

    JsonArray effects = doc.createNestedArray(F("effect_list"));
    mySettings->writeEffectsMqtt(effects);

    Serial.println(F("Sending discovery"));
    Serial.println(configTopic);
    serializeJsonPretty(doc, Serial);
    Serial.println();

    boolean success = sendJson(configTopic.c_str(), doc);
    Serial.printf_P(PSTR("Discovery sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

#if defined(ESP8266)
void callback(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t length, size_t index, size_t total)
#else
void callback(char* topic, byte* payload, unsigned int length)
#endif
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

    mySettings->processCommandMqtt(json);

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

bool connectToMqtt() {
    Serial.println(F("Connecting to MQTT..."));
#if defined(ESP8266)
    client->connect();
    return true;
#else
    client->connect(
        clientId.c_str(),
        mySettings->mqttSettings.username.c_str(),
        mySettings->mqttSettings.password.c_str(),
        availabilityTopic.c_str(),
        1,
        true,
        PSTR("false"));
    if (client->connected()) {
        onMqttConnect(true);
    }
    return client->connected();
#endif
}

#if defined(ESP8266)
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
         connectToMqtt();
    }
}
#endif

}

MqttClient *MqttClient::instance()
{
    return object;
}

void MqttClient::Initialize()
{
    if (object) {
        return;
    }

    Serial.println(F("Initializing MqttClient"));
    object = new MqttClient();
}

void MqttClient::loop()
{
#if defined(ESP8266)
    return;
#else
    if (!client) {
        return;
    }

    if (client->connected()) {
        client->loop();
    } else {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            // Attempt to reconnect
            if (connectToMqtt()) {
                lastReconnectAttempt = 0;
            }
        }
    }
#endif
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
    clientId = String(F("FireLampClient-")) + mySettings->connectionSettings.mdns;

#if defined(ESP8266)
    client = new AsyncMqttClient;
    client->onConnect(onMqttConnect);
    client->onDisconnect(onMqttDisconnect);
    client->onMessage(callback);
    client->setClientId(clientId.c_str());
    client->setWill(availabilityTopic.c_str(),
                    1,
                    true,
                    "false",
                    5);
    client->setCredentials(mySettings->mqttSettings.username.c_str(),
                           mySettings->mqttSettings.password.c_str());
#else
    static WiFiClient wifi;
    client = new PubSubClient(wifi);
    client->setCallback(callback);
#endif
    client->setServer(mySettings->mqttSettings.host.c_str(),
                      mySettings->mqttSettings.port);
    connectToMqtt();
}
