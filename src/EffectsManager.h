#pragma once
#include <Arduino.h>
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>

#define effectsManager EffectsManager::Instance()

class Effect;
class EffectsManager
{
public:
    static EffectsManager *Instance();
    static void Initialize();

    void Process();

    void ProcessEffectSettings(const JsonObject &json);

    void Next();
    void Previous();
    void ChangeEffect(uint8_t index);
    void ActivateEffect(uint8_t index);

    void UpdateCurrentSettings(const JsonObject &json);

    uint8_t Count();
    String EffectName(uint8_t index);

    Effect *activeEffect();
    uint8_t ActiveEffectIndex();

    bool working = true;

protected:
    EffectsManager();
};

