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
    void ChangeEffectByName(const String &name);
    void ActivateEffect(uint8_t index);

    void UpdateCurrentSettings(const JsonObject &json);
    void UpdateSettingsById(const String &id, const JsonObject &json);

    uint8_t Count();

    Effect *activeEffect();
    uint8_t ActiveEffectIndex();

    std::vector<Effect*> effects = {};

protected:
    EffectsManager();
};

