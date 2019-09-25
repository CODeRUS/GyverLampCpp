#pragma once
#include <Arduino.h>
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>

#define mySettings Settings::Instance()

class Settings
{
public:
    static Settings *Instance();
    static void Initialize(uint32_t saveInterval = 3000);

    DynamicJsonDocument &Json();

    void Process();
    void Reset();
    void SaveLater();
    void Save();

    String GetCurrentConfig();
    size_t GetCurrentConfigSize();
    void WriteConfigTo(char *buffer, size_t length);

    void ProcessConfig(const String &message);

    const char* GetCharField(const __FlashStringHelper *group, const __FlashStringHelper *field, const char *defaultValue);
    uint8_t GetByteField(const __FlashStringHelper *group, const __FlashStringHelper *field, uint8_t defaultValue);
    uint32_t GetULongLongField(const __FlashStringHelper *group, const __FlashStringHelper *field, uint32_t defaultValue);
    int GetIntField(const __FlashStringHelper *group, const __FlashStringHelper *field, int defaultValue);

    JsonArray GetEffects();

protected:
    Settings(uint32_t saveInterval = 3000);
};

