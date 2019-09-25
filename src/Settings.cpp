#include "Settings.h"
#include "EffectsManager.h"
#include "MyMatrix.h"
#include "LocalDNS.h"

#if defined(ESP32)
#include <SPIFFS.h>
#else
#include <FS.h>
#endif

namespace {

Settings *instance = nullptr;

bool settingsChanged = false;
uint32_t settingsSaveTimer = 0;
uint32_t settingsSaveInterval = 3000;

const char* settingsFileName PROGMEM = "/settings.json";
DynamicJsonDocument json(5130);

} // namespace

Settings *Settings::Instance()
{
    return instance;
}

void Settings::Initialize(uint32_t saveInterval)
{
    if (instance) {
        return;
    }

    Serial.println(F("Initializing Settings"));
    instance = new Settings(saveInterval);
}

DynamicJsonDocument &Settings::Json()
{
    return json;
}

void Settings::Process()
{
    if (settingsChanged && (millis() - settingsSaveTimer) > settingsSaveInterval) {
        settingsChanged = false;
        settingsSaveTimer = millis();
        Save();
    }
}

void Settings::SaveLater()
{
    settingsChanged = true;
    settingsSaveTimer = millis();
}

void Settings::Save()
{
    File file = SPIFFS.open(settingsFileName, "w");
    if (!file) {
        Serial.println(F("Error opening settings file from SPIFFS!"));
        return;
    }

    if (serializeJson(json, file) == 0) {
        Serial.println(F("Failed to write to file"));
    }

    if (file) {
        file.close();
    }
}

String Settings::GetCurrentConfig()
{
    json[F("working")] = effectsManager->working;

    String output;
    serializeJson(json, output);

    Serial.print(">> ");
    Serial.println(output);
    return output;
}

size_t Settings::GetCurrentConfigSize()
{
    return measureJson(json);
}

void Settings::WriteConfigTo(char *buffer, size_t length)
{
    serializeJson(json, buffer, length);
    Serial.print(F(">> "));
    Serial.println(buffer);
}

void Settings::ProcessConfig(const String &message)
{
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, message);

    const String event = doc[F("event")];
    if (event == F("WORKING")) {
        const bool working = doc[PSTR("data")];

        Serial.printf_P(PSTR("working: %s\n"), working ? PSTR("true") : PSTR("false"));
        effectsManager->working = working;
        if (!working) {
            myMatrix->clear();
            myMatrix->show();
        }
    } else if (event == F("ACTIVE_EFFECT")) {
        const int index = doc[F("data")];
        effectsManager->ChangeEffect(static_cast<uint8_t>(index));
    } else if (event == F("EFFECTS_CHANGED")) {
        const JsonObject effect = doc[F("data")];
        effectsManager->UpdateCurrentSettings(effect);
        SaveLater();
    } else if (event == F("ALARMS_CHANGED")) {

    }
}

const char *Settings::GetCharField(const __FlashStringHelper *group, const __FlashStringHelper *field, const char *defaultValue)
{
    JsonObject root = json.as<JsonObject>();
    if (group && !root.containsKey(group)) {
        return defaultValue;
    }
    JsonObject groupObject = group ? root[group] : root;
    if (!groupObject.containsKey(field)) {
        return defaultValue;
    }
    const char * value = groupObject[field];
    return value;
}

uint8_t Settings::GetByteField(const __FlashStringHelper *group, const __FlashStringHelper *field, uint8_t defaultValue)
{
    JsonObject root = json.as<JsonObject>();
    if (group && !root.containsKey(group)) {
        return defaultValue;
    }
    JsonObject groupObject = group ? root[group] : root;
    if (!groupObject.containsKey(field)) {
        return defaultValue;
    }
    uint8_t value = groupObject[field];
    return value;
}

uint32_t Settings::GetULongLongField(const __FlashStringHelper *group, const __FlashStringHelper *field, uint32_t defaultValue)
{
    JsonObject root = json.as<JsonObject>();
    if (group && !root.containsKey(group)) {
        return defaultValue;
    }
    JsonObject groupObject = group ? root[group] : root;
    if (!groupObject.containsKey(field)) {
        return defaultValue;
    }
    uint32_t value = groupObject[field];
    return value;
}

int Settings::GetIntField(const __FlashStringHelper *group, const __FlashStringHelper *field, int defaultValue)
{
    JsonObject root = json.as<JsonObject>();
    if (group && !root.containsKey(group)) {
        return defaultValue;
    }
    JsonObject groupObject = group ? root[group] : root;
    if (!groupObject.containsKey(field)) {
        return defaultValue;
    }
    int value = groupObject[field];
    return value;
}

JsonArray Settings::GetEffects()
{
    JsonObject root = json.as<JsonObject>();
    JsonArray effects = root[F("effects")];
    return effects;
}

Settings::Settings(uint32_t saveInterval)
{
    settingsSaveInterval = saveInterval;

    bool settingsExists = SPIFFS.exists(settingsFileName);
    Serial.printf_P(PSTR("SPIFFS Settings file exists: %s\n"), settingsExists ? PSTR("true") : PSTR("false"));
    if (!settingsExists) {
        Save();
        return;
    }

    File settings = SPIFFS.open(settingsFileName, "r");
    Serial.printf_P(PSTR("SPIFFS Settings file size: %zu\n"), settings.size());
    if (!settings) {
        Serial.println(F("SPIFFS Error reading settings file"));
        return;
    }

    DeserializationError err = deserializeJson(json, settings);
    settings.close();
    if (err) {
        Serial.print(F("SPIFFS Error parsing json file: "));
        Serial.println(err.c_str());
        return;
    }
}
