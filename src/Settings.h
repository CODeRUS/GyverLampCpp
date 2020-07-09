#pragma once
#include <Arduino.h>
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>

#define mySettings Settings::instance()

class AsyncWebSocket;
class AsyncWebSocketClient;
class Settings
{
public:
    struct EffectSettings {
        String id;
        String name;
        uint8_t speed = 1;
        uint8_t scale = 100;
        uint8_t brightness = 80;
    };

    struct GeneralSettings {
        uint8_t activeEffect = 0;
        bool working = true;
        bool soundControl = false;
        uint32_t logInterval = 0;
    };

    struct MatrixSettings {
        uint8_t width = 16;
        uint8_t height = 16;
        uint8_t segments = 1;
        // NEO_MATRIX_BOTTOM + RIGHT + COLUMNS + ZIGZAG
        uint8_t type = 15;
        uint8_t maxBrightness = 80;
        uint16_t currentLimit = 1000;
        uint8_t rotation = 3;
    };

    struct ConenctionSettings {
        String mdns;
        String apName;
        String ntpServer;
        uint32_t ntpOffset = 10800;
        String hostname;
    };

    struct MqttSettings {
        String host;
        uint16_t port = 1883;
        String username;
        String password;
        String uniqueId;
        String name;
        String manufacturer;
        String model;
    };

    struct ButttonSettings {
        uint8_t pin = 0;
        uint8_t type = 1;
        uint8_t state = 0;
    };

    static Settings *instance();
    static void Initialize(uint32_t saveInterval = 3000);

    size_t jsonSerializeSize();

    void loop();
    void reset();
    void saveLater();
    void saveSettings();
    void saveEffects();

    void buildSettingsJson(JsonObject &root);
    void buildEffectsJson(JsonArray &effects);
    void buildJsonMqtt(JsonObject &root);
    void writeEffectsMqtt(JsonArray &array);

    void processConfig(const String &message);
    void processCommandMqtt(const JsonObject &json);

    bool readSettings();
    bool readEffects();

    GeneralSettings generalSettings;
    MatrixSettings matrixSettings;
    ConenctionSettings connectionSettings;
    MqttSettings mqttSettings;
    ButttonSettings buttonSettings;

protected:
    Settings(uint32_t saveInterval = 3000);
};

