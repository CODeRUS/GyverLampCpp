#pragma once
#include <Arduino.h>
#include <EEPROM.h>

class Settings
{
public:
    struct AlarmSettings {
        bool enabled = false;
        uint64_t time = 0;
    };

    struct EffectSettings {
        uint8_t effectBrightness = 80;
        uint8_t effectSpeed = 10;
        uint8_t effectScale = 1;
    };

    static void Initialize(const uint8_t eepromInitialization, uint32_t saveInterval = 3000);
    static void Process();
    static void Reset();
    static void SaveLater();
    static void Save();

    static EffectSettings *CurrentEffectSettings();

    static uint8_t initializationFlag;
    static AlarmSettings* alarmSettings;
    static uint8_t currentEffect;
    static EffectSettings* effectsSettings;
    static uint8_t dawnMode;
    static bool masterSwitch;
};

