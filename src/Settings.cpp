#include "Settings.h"
#include "EffectsManager.h"
#include "MyMatrix.h"
#include "LocalDNS.h"
#include "MqttClient.h"
#include "LampWebServer.h"

#include <ESPAsyncWebServer.h>

#if defined(ESP32)
#include <SPIFFS.h>
#else
#include <FS.h>
#endif
#include "effects/Effect.h"

namespace {

const size_t jsonSerializeSize = 512 * 22;

Settings *instance = nullptr;

bool settingsChanged = false;
uint32_t settingsSaveTimer = 0;
uint32_t settingsSaveInterval = 3000;

const char* settingsFileName PROGMEM = "/settings.json";

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

String Settings::GetUniqueID()
{
#if defined(ESP32)
  return String((uint32_t)ESP.getEfuseMac(), HEX);
#else
  return String((uint32_t)ESP.getChipId(), HEX);
#endif
}

size_t Settings::JsonSerializeSize()
{
    return jsonSerializeSize;
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

    DynamicJsonDocument json(jsonSerializeSize);
    JsonObject root = json.to<JsonObject>();
    BuildJson(root);

    if (serializeJson(json, file) == 0) {
        Serial.println(F("Failed to write to file"));
    }

    if (file) {
        file.close();
    }
}

void Settings::WriteEffectsMqtt(JsonArray &array)
{
    for (Effect *effect : effectsManager->effects) {
        array.add(effect->settings.name);
    }
}

void Settings::ProcessConfig(const String &message)
{
    DynamicJsonDocument doc(512);
    deserializeJson(doc, message);

    const String event = doc[F("event")];
    if (event == F("WORKING")) {
        const bool working = doc[F("data")];

        Serial.printf_P(PSTR("working: %s\n"), working ? PSTR("true") : PSTR("false"));
        mySettings->generalSettings.working = working;
    } else if (event == F("ACTIVE_EFFECT")) {
        const int index = doc[F("data")];
        effectsManager->ChangeEffect(static_cast<uint8_t>(index));
    } else if (event == F("EFFECTS_CHANGED")) {
        const JsonObject effect = doc[F("data")];
        effectsManager->UpdateCurrentSettings(effect);
        SaveLater();
    } else if (event == F("ALARMS_CHANGED")) {

    }

    mqtt->update();
}

void Settings::ProcessCommandMqtt(const JsonObject &json)
{
    if (json.containsKey(F("state"))) {
        const String state = json[F("state")];
        mySettings->generalSettings.working = state == F("ON");
    }
    if (json.containsKey(F("effect"))) {
        const String effect = json[F("effect")];
        effectsManager->ChangeEffectByName(effect);
    }
    effectsManager->UpdateCurrentSettings(json);
    SaveLater();

    lampWebServer->Update();
}

void Settings::BuildJson(JsonObject &root)
{
    JsonArray effects = root.createNestedArray(F("effects"));
    for (Effect *effect : effectsManager->effects) {
        JsonObject effectObject = effects.createNestedObject();
        effectObject[F("id")] = effect->settings.id;
        effectObject[F("name")] = effect->settings.name;
        effectObject[F("speed")] = effect->settings.speed;
        effectObject[F("scale")] = effect->settings.scale;
        effectObject[F("brightness")] = effect->settings.brightness;
        effect->writeSettings(effectObject);
    }
    root[F("activeEffect")] = effectsManager->ActiveEffectIndex();
    root[F("working")] = generalSettings.working;

    JsonObject matrixObject = root.createNestedObject(F("matrix"));
    matrixObject[F("width")] = matrixSettings.width;
    matrixObject[F("height")] = matrixSettings.height;
    matrixObject[F("segments")] = matrixSettings.segments;
    matrixObject[F("type")] = matrixSettings.type;
    matrixObject[F("maxBrightness")] = matrixSettings.maxBrightness;
    matrixObject[F("currentLimit")] = matrixSettings.currentLimit;
    matrixObject[F("rotation")] = matrixSettings.rotation;

    JsonObject connectionObject = root.createNestedObject(F("connection"));
    connectionObject[F("uniqueId")] = connectionSettings.uniqueId;
    connectionObject[F("mdns")] = connectionSettings.mdns;
    connectionObject[F("apName")] = connectionSettings.apName;
    connectionObject[F("ntpServer")] = connectionSettings.ntpServer;
    connectionObject[F("ntpOffset")] = connectionSettings.ntpOffset;

    JsonObject mqttObject = root.createNestedObject(F("mqtt"));
    mqttObject[F("host")] = mqttSettings.host;
    mqttObject[F("port")] = mqttSettings.port;
    mqttObject[F("username")] = mqttSettings.username;
    mqttObject[F("password")] = mqttSettings.password;

    JsonObject spectrometerObject = root.createNestedObject(F("spectrometer"));
    spectrometerObject[F("active")] = generalSettings.soundControl;
}

void Settings::BuildEffectsJson(JsonObject &root)
{
    JsonArray effects = root.createNestedArray(F("effects"));
    for (Effect *effect : effectsManager->effects) {
        JsonObject effectObject = effects.createNestedObject();
        effectObject[F("id")] = effect->settings.id;
        effectObject[F("name")] = effect->settings.name;
        effectObject[F("speed")] = effect->settings.speed;
        effectObject[F("scale")] = effect->settings.scale;
        effectObject[F("brightness")] = effect->settings.brightness;
        effect->writeSettings(effectObject);
    }
}

void Settings::BuildJsonMqtt(JsonObject &root)
{
    root[F("state")] = generalSettings.working ? F("ON") : F("OFF");
    root[F("brightness")] = effectsManager->activeEffect()->settings.brightness;
    root[F("speed")] = effectsManager->activeEffect()->settings.speed;
    root[F("scale")] = effectsManager->activeEffect()->settings.scale;
    root[F("effect")] = effectsManager->activeEffect()->settings.name;
    effectsManager->activeEffect()->writeSettings(root);
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

    DynamicJsonDocument json(jsonSerializeSize); // ICE: compute with https://arduinojson.org/v6/assistant/
    DeserializationError err = deserializeJson(json, settings);
    settings.close();
    if (err) {
        Serial.print(F("SPIFFS Error parsing json file: "));
        Serial.println(err.c_str());
        return;
    }

    JsonObject root = json.as<JsonObject>();
    if (root.containsKey(F("matrix"))) {
       JsonObject matrixObject = root[F("matrix")];
       if (matrixObject.containsKey(F("width"))) {
           matrixSettings.width = matrixObject[F("width")];
       }
       if (matrixObject.containsKey(F("height"))) {
           matrixSettings.height = matrixObject[F("height")];
       }
       if (matrixObject.containsKey(F("segments"))) {
           matrixSettings.segments = matrixObject[F("segments")];
       }
       if (matrixObject.containsKey(F("type"))) {
           matrixSettings.type = matrixObject[F("type")];
       }
       if (matrixObject.containsKey(F("maxBrightness"))) {
           matrixSettings.maxBrightness = matrixObject[F("maxBrightness")];
       }
       if (matrixObject.containsKey(F("currentLimit"))) {
           matrixSettings.currentLimit = matrixObject[F("currentLimit")];
       }
       if (matrixObject.containsKey(F("rotation"))) {
           matrixSettings.rotation = matrixObject[F("rotation")];
       }
    }

    if (root.containsKey(F("connection"))) {
       JsonObject connectionObject = root[F("connection")];
       if (connectionObject.containsKey(F("uniqueId"))) {
           connectionSettings.uniqueId = connectionObject[F("uniqueId")].as<String>();
       }
       if (connectionObject.containsKey(F("mdns"))) {
           connectionSettings.mdns = connectionObject[F("mdns")].as<String>();
       }
       if (connectionObject.containsKey(F("apName"))) {
           connectionSettings.apName = connectionObject[F("apName")].as<String>();
       }
       if (connectionObject.containsKey(F("ntpServer"))) {
           connectionSettings.ntpServer = connectionObject[F("ntpServer")].as<String>();
       }
       if (connectionObject.containsKey(F("ntpOffset"))) {
           connectionSettings.ntpOffset = connectionObject[F("ntpOffset")];
       }
    }

    if (root.containsKey(F("mqtt"))) {
       JsonObject mqttObject = root[F("mqtt")];
       if (mqttObject.containsKey(F("host"))) {
           mqttSettings.host = mqttObject[F("host")].as<String>();
       }
       if (mqttObject.containsKey(F("port"))) {
           mqttSettings.port = mqttObject[F("port")];
       }
       if (mqttObject.containsKey(F("username"))) {
           mqttSettings.username = mqttObject[F("username")].as<String>();
       }
       if (mqttObject.containsKey(F("password"))) {
           mqttSettings.password = mqttObject[F("password")].as<String>();
       }
    }

    if (root.containsKey(F("spectrometer"))) {
       JsonObject spectrometerObject = root[F("spectrometer")];
       if (spectrometerObject.containsKey(F("active"))) {
           generalSettings.soundControl = spectrometerObject[F("active")];
       }
    }

    if (root.containsKey(F("effects"))) {
        JsonArray effects = root[F("effects")];
        for (JsonObject effect : effects) {
            effectsManager->ProcessEffectSettings(effect);
        }
    }

    if (root.containsKey(F("activeEffect"))) {
        generalSettings.activeEffect = root[F("activeEffect")];
    }
}
