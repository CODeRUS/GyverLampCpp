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

const size_t jsonSerializeSize = 512 * 20;

Settings *instance = nullptr;

bool settingsChanged = false;
uint32_t settingsSaveTimer = 0;
uint32_t settingsSaveInterval = 3000;

const char* settingsFileName PROGMEM = "/settings.json";
const char* effectsFileName PROGMEM = "/effects.json";

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
        SaveSettings();
        SaveEffects();
    }
}

void Settings::SaveLater()
{
    lampWebServer->Update();
    settingsChanged = true;
    settingsSaveTimer = millis();
}

void Settings::SaveSettings()
{
    File file = SPIFFS.open(settingsFileName, "w");
    if (!file) {
        Serial.println(F("Error opening settings file from SPIFFS!"));
        return;
    }

    DynamicJsonDocument json(jsonSerializeSize);
    JsonObject root = json.to<JsonObject>();
    BuildSettingsJson(root);

    if (serializeJson(json, file) == 0) {
        Serial.println(F("Failed to write settings to file"));
    }

    if (file) {
        file.close();
    }
}

void Settings::SaveEffects()
{
    File file = SPIFFS.open(effectsFileName, "w");
    if (!file) {
        Serial.println(F("Error opening effects file from SPIFFS!"));
        return;
    }

    DynamicJsonDocument json(jsonSerializeSize);
    JsonArray root = json.to<JsonArray>();
    BuildEffectsJson(root);

    if (serializeJson(json, file) == 0) {
        Serial.println(F("Failed to write effects to file"));
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
        const String id = effect[F("i")];
        if (id == effectsManager->activeEffect()->settings.id) {
            effectsManager->UpdateCurrentSettings(effect);
        } else {
            effectsManager->UpdateSettingsById(id, effect);
        }
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

void Settings::ReadSettings()
{
    bool settingsExists = SPIFFS.exists(settingsFileName);
    Serial.printf_P(PSTR("SPIFFS Settings file exists: %s\n"), settingsExists ? PSTR("true") : PSTR("false"));
    if (!settingsExists) {
        SaveSettings();
        return;
    }

    File settings = SPIFFS.open(settingsFileName, "r");
    Serial.printf_P(PSTR("SPIFFS Settings file size: %zu\n"), settings.size());
    if (!settings) {
        Serial.println(F("SPIFFS Error reading settings file"));
        return;
    }

    DynamicJsonDocument json(1024);
    DeserializationError err = deserializeJson(json, settings);
    settings.close();
    if (err) {
        Serial.print(F("SPIFFS Error parsing settings json file: "));
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

    if (root.containsKey(F("button"))) {
       JsonObject buttonObject = root[F("button")];
       if (buttonObject.containsKey(F("pin"))) {
           uint8_t btnPin = buttonObject[F("pin")];
           if (btnPin != 0) {
               buttonSettings.pin = btnPin;
           }
       }
       if (buttonObject.containsKey(F("type"))) {
           buttonSettings.type = buttonObject[F("type")];
       }
       if (buttonObject.containsKey(F("state"))) {
           buttonSettings.state = buttonObject[F("state")];
       }
    }

    if (root.containsKey(F("logInterval"))) {
        generalSettings.logInterval = root[F("logInterval")];
    }

    if (root.containsKey(F("activeEffect"))) {
        generalSettings.activeEffect = root[F("activeEffect")];
    }
}

void Settings::ReadEffects()
{
    bool effectsExists = SPIFFS.exists(effectsFileName);
    Serial.printf_P(PSTR("SPIFFS Effects file exists: %s\n"), effectsExists ? PSTR("true") : PSTR("false"));
    if (!effectsExists) {
//        SaveEffects();
        return;
    }

    File effects = SPIFFS.open(effectsFileName, "r");
    Serial.printf_P(PSTR("SPIFFS Effects file size: %zu\n"), effects.size());
    if (!effects) {
        Serial.println(F("SPIFFS Error reading effects file"));
        return;
    }

    DynamicJsonDocument json(jsonSerializeSize);
    DeserializationError err = deserializeJson(json, effects);
    effects.close();
    if (err) {
        Serial.print(F("SPIFFS Error parsing effects json file: "));
        Serial.println(err.c_str());
        return;
    }

    JsonArray root = json.as<JsonArray>();
    for (JsonObject effect : root) {
        effectsManager->ProcessEffectSettings(effect);
    }
}

void Settings::BuildSettingsJson(JsonObject &root)
{
    root[F("activeEffect")] = effectsManager->ActiveEffectIndex();
    root[F("logInterval")] = generalSettings.logInterval;
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

void Settings::BuildEffectsJson(JsonArray &effects)
{
    for (Effect *effect : effectsManager->effects) {
        JsonObject effectObject = effects.createNestedObject();
        effectObject[F("i")] = effect->settings.id;
        effectObject[F("n")] = effect->settings.name;
        effectObject[F("s")] = effect->settings.speed;
        effectObject[F("l")] = effect->settings.scale;
        effectObject[F("b")] = effect->settings.brightness;
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

    connectionSettings.mdns = F("firelamp");
    connectionSettings.apName = F("Fire Lamp");
    connectionSettings.ntpServer = F("europe.pool.ntp.org");
    connectionSettings.manufacturer = F("coderus");
}
