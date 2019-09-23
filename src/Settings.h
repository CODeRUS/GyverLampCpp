#pragma once
#include <Arduino.h>

#define mySettings Settings::Instance()

class Settings
{
public:
    static Settings *Instance();
    static void Initialize(uint32_t saveInterval = 3000);

    struct AlarmSettings {
        bool enabled = false;
        uint64_t time = 0;
    };

    struct EffectSettings {
        uint8_t effectBrightness = 80;
        uint8_t effectSpeed = 10;
        uint8_t effectScale = 1;
    };

    void Process();
    void Reset();
    void SaveLater();
    void Save();

    String GetCurrentConfig();
    void ApplyConfig(const String &message);

    EffectSettings *CurrentEffectSettings();

    uint8_t initializationFlag = 0;
    AlarmSettings* alarmSettings = nullptr;
    uint8_t currentEffect = 0;
    EffectSettings* effectsSettings = nullptr;
    uint8_t dawnMode = 0;
    uint8_t matrixRotation = 0;
    bool masterSwitch = true;

protected:
    Settings(uint32_t saveInterval = 3000);
};

