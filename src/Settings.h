#pragma once
#include <Arduino.h>
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>

#define mySettings Settings::Instance()

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
        String uniqueId = GetUniqueID();
        String mdns;
        String apName;
        String ntpServer;
        uint32_t ntpOffset = 10800;
        String manufacturer;
    };

    struct MqttSettings {
        String host;
        uint16_t port = 1883;
        String username;
        String password;
    };

    struct ButttonSettings {
        uint8_t pin = 0;
        uint8_t type = 1;
        uint8_t state = 0;
    };

    static Settings *Instance();
    static void Initialize(uint32_t saveInterval = 3000);
    static String GetUniqueID();
    static size_t JsonSerializeSize();

    void Process();
    void Reset();
    void SaveLater();
    void SaveSettings();
    void SaveEffects();

    void BuildSettingsJson(JsonObject &root);
    void BuildEffectsJson(JsonArray &effects);
    void BuildJsonMqtt(JsonObject &root);
    void WriteEffectsMqtt(JsonArray &array);

    void ProcessConfig(const String &message);
    void ProcessCommandMqtt(const JsonObject &json);

    void ReadSettings();
    void ReadEffects();

    GeneralSettings generalSettings;
    MatrixSettings matrixSettings;
    ConenctionSettings connectionSettings;
    MqttSettings mqttSettings;
    ButttonSettings buttonSettings;

protected:
    Settings(uint32_t saveInterval = 3000);
};

