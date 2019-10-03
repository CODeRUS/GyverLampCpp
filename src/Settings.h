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
        String id = "";
        String name = "";
        uint8_t speed = 1;
        uint8_t scale = 100;
        uint8_t brightness = 80;
    };

    struct GeneralSettings {
        uint8_t activeEffect = 0;
        bool working = true;
        bool soundControl = false;
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
        String mdns = "firelamp";
        String apName = "Fire Lamp";
        String ntpServer = "europe.pool.ntp.org";
        uint32_t ntpOffset = 10800;
    };

    static Settings *Instance();
    static void Initialize(uint32_t saveInterval = 3000);

    void Process();
    void Reset();
    void SaveLater();
    void Save();

    void BuildJson(JsonObject &root);
    void WriteConfigTo(AsyncWebSocket *socket, AsyncWebSocketClient *client);

    void ProcessConfig(const String &message);

    GeneralSettings generalSettings;
    MatrixSettings matrixSettings;
    ConenctionSettings connectionSettings;

protected:
    Settings(uint32_t saveInterval = 3000);
};

