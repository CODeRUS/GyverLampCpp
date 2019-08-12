#include "Settings.h"
#include "EffectsManager.h"
#include "MyMatrix.h"

#include <ArduinoJson.h>

uint8_t Settings::initializationFlag = 0;
Settings::AlarmSettings* Settings::alarmSettings;
uint8_t Settings::currentEffect = 0;
Settings::EffectSettings* Settings::effectsSettings;
uint8_t Settings::dawnMode = 0;
bool Settings::masterSwitch = true;

namespace {

size_t eepromSize = 0;

const uint8_t alarmsCount = 7;
bool settingsChanged = false;
uint32_t eepromTimer = 0;
uint32_t eepromSaveInterval = 3000;

} // namespace

void Settings::Initialize(const uint8_t eepromInitialization, uint32_t saveInterval)
{
    eepromSaveInterval = saveInterval;

    alarmSettings = new AlarmSettings[alarmsCount]();
    effectsSettings = new EffectSettings[EffectsManager::Count()]();

    eepromSize = sizeof(uint8_t) // initializationFlag
               + sizeof(AlarmSettings) * alarmsCount // alarmSettings
               + sizeof(uint8_t) // currentEffect
               + sizeof(EffectSettings) * EffectsManager::Count() // effectsSettings
               + sizeof(uint8_t); // dawnMode
    Serial.printf("EEPROM size used: %zu\n", eepromSize);

    int address = 0;

#if defined(ESP32)
    const bool eepromReady =
#endif
     EEPROM.begin(eepromSize);

#if defined(ESP32)
    Serial.printf("EEPROM ready: %s\n", eepromReady ? "true" : "false");
#endif

    initializationFlag = EEPROM.read(address);
    Serial.printf("initialization read: %u, expected: %u\n", initializationFlag, eepromInitialization);
    if (eepromInitialization != initializationFlag) {
        initializationFlag = eepromInitialization;
        Serial.println("Erasing EEPROM");
        Save();
        return;
    }

    address += sizeof(uint8_t);

    for (int i = 0; i < alarmsCount; i++) {
        EEPROM.get(address, alarmSettings[i]);
        address += sizeof(AlarmSettings);
    }

    currentEffect = EEPROM.read(address);
    address += sizeof(uint8_t);

    for (uint8_t i = 0; i < EffectsManager::Count(); i++) {
        EEPROM.get(address, effectsSettings[i]);
        Serial.printf("Read effect %s, speed: %u, scale: %u, brightness: %u\n",
                      EffectsManager::EffectName(i).c_str(),
                      effectsSettings[i].effectSpeed,
                      effectsSettings[i].effectScale,
                      effectsSettings[i].effectBrightness);
        address += sizeof(EffectSettings);
        delay(10);
    }

    dawnMode = EEPROM.read(address);
    address += sizeof(uint8_t);
}

void Settings::Process()
{
    if (settingsChanged && (millis() - eepromTimer) > eepromSaveInterval) {
        settingsChanged = false;
        eepromTimer = millis();
        Save();
    }
}

void Settings::SaveLater()
{
    settingsChanged = true;
    eepromTimer = millis();
}

void Settings::Save()
{
    Serial.println("Saving EEPROM");

    int address = 0;

    EEPROM.write(address, initializationFlag);
    address += sizeof(uint8_t);

    for (int i = 0; i < alarmsCount; i++) {
        EEPROM.put(address, alarmSettings[i]);
        address += sizeof(AlarmSettings);
    }

    EEPROM.write(address, currentEffect);
    address += sizeof(uint8_t);

    for (uint8_t i = 0; i < EffectsManager::Count(); i++) {
        Serial.printf("Write effect %s, speed: %u, scale: %u, brightness: %u\n",
                      EffectsManager::EffectName(i).c_str(),
                      effectsSettings[i].effectSpeed,
                      effectsSettings[i].effectScale,
                      effectsSettings[i].effectBrightness);
        EEPROM.put(address, effectsSettings[i]);
        address += sizeof(EffectSettings);
    }

    EEPROM.write(address, dawnMode);
    address += sizeof(uint8_t);

    EEPROM.commit();
}

String Settings::GetCurrentConfig()
{
    DynamicJsonDocument doc(4096);

    doc["working"] = masterSwitch;
    doc["activeEffect"] = currentEffect;

    JsonArray effects = doc.createNestedArray("effects");

    for (uint8_t index = 0; index < EffectsManager::Count(); index++) {
        const EffectSettings settings = effectsSettings[index];

        JsonObject effect = effects.createNestedObject();
        effect["name"] = EffectsManager::EffectName(index);
        effect["speed"] = settings.effectSpeed;
        effect["scale"] = settings.effectScale;
        effect["brightness"] = settings.effectBrightness;

    }

    JsonArray alarms = doc.createNestedArray("alarms");
//    alarms.add(48.756080);
//    alarms.add(2.302038);

    String output;
    serializeJson(doc, output);

    Serial.print(">> ");
    Serial.println(output);
    return output;
}

void Settings::ApplyConfig(const String &message)
{
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, message);

    const String event = doc["event"];
    if (event == "WORKING") {
        const bool working = doc["data"];

        Serial.printf("working: %s\n", working ? "true" : "false");
        masterSwitch = working;
        if (!masterSwitch) {
            myMatrix->clear();
            myMatrix->show();
        }
    } else if (event == "ACTIVE_EFFECT") {
        const int index = doc["data"];
        EffectsManager::ChangeEffect(static_cast<uint8_t>(index));
    } else if (event == "EFFECTS_CHANGED") {
        const JsonObject effect = doc["data"];
        const int speed = effect["speed"];
        const int scale = effect["scale"];
        const int brightness = effect["brightness"];
        CurrentEffectSettings()->effectSpeed = static_cast<uint8_t>(speed);
        CurrentEffectSettings()->effectScale = static_cast<uint8_t>(scale);
        CurrentEffectSettings()->effectBrightness = static_cast<uint8_t>(brightness);
        myMatrix->setBrightness(brightness);
        SaveLater();
    } else if (event == "ALARMS_CHANGED") {

    }
}

Settings::EffectSettings* Settings::CurrentEffectSettings()
{
    return &effectsSettings[currentEffect];
}
