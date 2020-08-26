#include "MqttClient.h"

#include <Arduino.h>
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <Ticker.h>
#include <PangolinMQTT.h>

#include "Settings.h"

namespace
{

Ticker mqttReconnectTimer;

MqttClient *object = nullptr;
PangolinMQTT *client = nullptr;

String commonTopic;

String availabilityTopic;
String configTopic;
String setTopic;
String stateTopic;

String clientId;

void subscribe()
{
    if (!client->connected()) {
        return;
    }

    Serial.print(F("Subscribing to topic: "));
    Serial.println(setTopic);
    client->subscribe(setTopic.c_str(), 0);
}

void sendString(String topic, String message, uint8_t qos = 2, bool retain = false)
{
    if (!client->connected()) {
        return;
    }

    client->publish(topic.c_str(), qos, retain, (uint8_t*)message.c_str(), message.length(), false);
}

bool sendJson(String topic, const DynamicJsonDocument &doc)
{
    if (!client->connected()) {
        return false;
    }

    String buffer;
    if (!serializeJson(doc, buffer)) {
        Serial.println(F("writing payload: wrong size!"));
        return false;
    }
    sendString(topic, buffer);
    return true;
}

void sendState()
{
    if (!client->connected()) {
        return;
    }

    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();

    mySettings->buildJsonMqtt(json);

    Serial.println(F("Sending state"));
    Serial.println(stateTopic);
    serializeJsonPretty(doc, Serial);
    Serial.println();

    boolean success = sendJson(stateTopic, doc);
    Serial.printf_P(PSTR("State sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

void sendAvailability()
{
    if (!client->connected()) {
        return;
    }

    Serial.println(F("Sending availability"));
    Serial.println(availabilityTopic);
    sendString(availabilityTopic, F("true"), 2, true);
}

void sendDiscovery()
{
    if (!client->connected()) {
        return;
    }

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
    doc[F("rgb")] = true;
    doc[F("json_attr_t")] =F("~/state");

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

    boolean success = sendJson(configTopic, doc);
    Serial.printf_P(PSTR("Discovery sent: %s\n"), success ? PSTR("success") : PSTR("fail"));
}

void callback(const char* topic, uint8_t* payload, struct PANGO_PROPS props, size_t len, size_t index, size_t total)
{
    Serial.println(topic);

    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, payload, len) != DeserializationError::Ok) {
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

void connectToMqtt() {
    if (!WiFi.isConnected()) {
        return;
    }

    if (!client) {
        return;
    }

    Serial.println(F("Connecting to MQTT..."));
    client->connect();
}

void onMqttDisconnect(int8_t reason)
{
    Serial.print(F("MQTT disconnect reason: "));
    switch (reason) {
    case TCP_DISCONNECTED:
        Serial.println(F("TCP_DISCONNECTED"));
        break;
    case MQTT_SERVER_UNAVAILABLE:
        Serial.println(F("MQTT_SERVER_UNAVAILABLE"));
        break;
    case UNRECOVERABLE_CONNECT_FAIL:
        Serial.println(F("UNRECOVERABLE_CONNECT_FAIL"));
        break;
    case TLS_BAD_FINGERPRINT:
        Serial.println(F("TLS_BAD_FINGERPRINT"));
        break;
    case TCP_TIMEOUT:
        Serial.println(F("TCP_TIMEOUT"));
        break;
    case SUBSCRIBE_FAIL:
        Serial.println(F("SUBSCRIBE_FAIL"));
        break;
    case INBOUND_QOS_FAIL:
        Serial.println(F("INBOUND_QOS_FAIL"));
        break;
    case OUTBOUND_QOS_FAIL:
        Serial.println(F("OUTBOUND_QOS_FAIL"));
        break;
    case INBOUND_QOS_ACK_FAIL:
        Serial.println(F("INBOUND_QOS_ACK_FAIL"));
        break;
    case OUTBOUND_QOS_ACK_FAIL:
        Serial.println(F("OUTBOUND_QOS_ACK_FAIL"));
        break;
    case INBOUND_PUB_TOO_BIG:
        Serial.println(F("INBOUND_PUB_TOO_BIG"));
        break;
    case OUTBOUND_PUB_TOO_BIG:
        Serial.println(F("OUTBOUND_PUB_TOO_BIG"));
        break;
    case BOGUS_PACKET:
        Serial.println(F("BOGUS_PACKET"));
        break;
    case BOGUS_ACK:
        Serial.println(F("BOGUS_ACK"));
        break;
    default:
        Serial.printf_P(PSTR("unknown %d\n"), reason);
    }
}

#if defined(ESP8266)
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
    Serial.println("Connected to Wi-Fi.");
    mqttReconnectTimer.once(2, connectToMqtt);
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
    Serial.println("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
}

#else
void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("Connected to Wi-Fi.");
        mqttReconnectTimer.once(2, connectToMqtt);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("Disconnected from Wi-Fi.");
        mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        break;
    default:
        break;
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

#ifdef ARDUINO_ARCH_ESP8266
    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
#else
    WiFi.onEvent(WiFiEvent);
#endif

    commonTopic = String(F("homeassistant/light/")) + mySettings->connectionSettings.mdns;
    setTopic = commonTopic + String(F("/set"));
    stateTopic = commonTopic + String(F("/state"));
    configTopic = commonTopic + String(F("/config"));
    availabilityTopic = commonTopic + String(F("/available"));
    clientId = String(F("FireLampClient-")) + mySettings->connectionSettings.mdns;

    client = new PangolinMQTT;
    client->onConnect(onMqttConnect);
    client->onDisconnect(onMqttDisconnect);
    client->onMessage(callback);
    client->setClientId(clientId.c_str());
    client->setWill(availabilityTopic.c_str(),
                    1,
                    true,
                    "false");
    client->setCredentials(mySettings->mqttSettings.username.c_str(),
                           mySettings->mqttSettings.password.c_str());
    client->setServer(mySettings->mqttSettings.host.c_str(),
                      mySettings->mqttSettings.port);

    mqttReconnectTimer.once(2, connectToMqtt);
}
