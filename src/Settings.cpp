#include "Settings.h"
#include "EffectsManager.h"
#include "MyMatrix.h"
#include "LocalDNS.h"
#include "MqttClient.h"
#include "LampWebServer.h"

#include <ESPAsyncWebServer.h>

#if defined(ESP32)
#include <SPIFFS.h>
#define FLASHFS SPIFFS
#else
#include <LittleFS.h>
#define FLASHFS LittleFS
#endif
#include "effects/Effect.h"

namespace {

const size_t serializeSize = 512 * 22;

Settings *object = nullptr;

bool settingsChanged = false;
uint32_t settingsSaveTimer = 0;
uint32_t settingsSaveInterval = 3000;

const char* settingsFileName PROGMEM = "/settings.json";
const char* effectsFileName PROGMEM = "/effects.json";

std::vector<String> pendingConfig;
std::vector<String> pendingCommand;

String GetUniqueID()
{
#if defined(ESP32)
  return String((uint32_t)ESP.getEfuseMac(), HEX);
#else
  return String((uint32_t)ESP.getChipId(), HEX);
#endif
}

} // namespace

Settings *Settings::instance()
{
    return object;
}

void Settings::Initialize(uint32_t saveInterval)
{
    if (object) {
        return;
    }

    Serial.println(F("Initializing Settings"));
    object = new Settings(saveInterval);
}

size_t Settings::jsonSerializeSize()
{
    return serializeSize;
}

void Settings::loop()
{
    if (settingsChanged && (millis() - settingsSaveTimer) > settingsSaveInterval) {
        settingsChanged = false;
        settingsSaveTimer = millis();
        saveSettings();
        saveEffects();

        if (pendingConfig.size()) {
            for (const String &config : pendingConfig) {
                processConfig(config);
            }
            pendingConfig.clear();
        }

        if (pendingCommand.size()) {
            for (const String &command : pendingCommand) {
                processCommandMqtt(command);
            }
            pendingCommand.clear();
        }
    }
}

void Settings::saveLater()
{
    settingsChanged = true;
    settingsSaveTimer = millis();
}

void Settings::saveSettings()
{
    busy = true;

    Serial.print(F("Saving settings... "));

    File file = FLASHFS.open(settingsFileName, "w");
    if (!file) {
        Serial.println(F("Error opening settings file from FLASHFS!"));
        return;
    }

    DynamicJsonDocument json(serializeSize);
    JsonObject root = json.to<JsonObject>();
    buildSettingsJson(root);

    if (serializeJson(json, file) == 0) {
        Serial.println(F("Failed to serialize settings"));
        saveLater();
    }

    if (file) {
        file.close();
    }
    Serial.println(F("Done!"));

    busy = false;
}

void Settings::saveEffects()
{
    busy = true;

    Serial.print(F("Saving effects... "));

    File file = FLASHFS.open(effectsFileName, "w");
    if (!file) {
        Serial.println(F("Error opening effects file from FLASHFS!"));
        return;
    }

    DynamicJsonDocument json(serializeSize);
    JsonArray root = json.to<JsonArray>();
    buildEffectsJson(root);

    if (serializeJson(json, file) == 0) {
        Serial.println(F("Failed to serialize effects"));
        saveLater();
    }
    if (file) {
        file.close();
    }
    Serial.println(F("Done!"));

    busy = false;
}

void Settings::writeEffectsMqtt(JsonArray &array)
{
    for (Effect *effect : effectsManager->effects) {
        array.add(effect->settings.name);
    }
}

void Settings::processConfig(const String &message)
{
    if (busy) {
        Serial.println(F("\nSaving in progress! Delaying operation.\n"));
        pendingConfig.push_back(message);
        lampWebServer->update();
        return;
    }

    Serial.print(F("<< "));
    Serial.println(message);

    {
        DynamicJsonDocument doc(512);
        if (DeserializationError err = deserializeJson(doc, message)) {
            Serial.print(F("[processConfig] Error parsing json: "));
            Serial.println(err.c_str());
            return;
        }

        const String event = doc[F("event")];
        if (event == F("WORKING")) {
            const bool working = doc[F("data")];
            Serial.printf_P(PSTR("working: %s\n"), working ? PSTR("true") : PSTR("false"));
            mySettings->generalSettings.working = working;
        } else if (event == F("ACTIVE_EFFECT")) {
            const int index = doc[F("data")];
            effectsManager->activateEffect(static_cast<uint8_t>(index));
        } else if (event == F("EFFECTS_CHANGED")) {
            const JsonObject effect = doc[F("data")];
            const String id = effect[F("i")];
            if (id == effectsManager->activeEffect()->settings.id) {
                effectsManager->updateCurrentSettings(effect);
            } else {
                effectsManager->updateSettingsById(id, effect);
            }
            saveLater();
        } else if (event == F("ALARMS_CHANGED")) {

        }
    }

    mqtt->update();
    lampWebServer->update();
}

void Settings::processCommandMqtt(const String &message)
{
    if (busy) {
        Serial.println(F("\nSaving in progress! Delaying operation.\n"));
        pendingCommand.push_back(message);
        return;
    }

    Serial.println(message);
    Serial.println();

    {
        DynamicJsonDocument doc(1024);
        if (DeserializationError err = deserializeJson(doc, message)) {
            Serial.print(F("[processCommandMqtt] Error parsing json: "));
            Serial.println(err.c_str());
        }
        JsonObject json = doc.as<JsonObject>();

        if (json.containsKey(F("state"))) {
            const String state = json[F("state")];
            mySettings->generalSettings.working = state == F("ON");

            if (json.containsKey(F("effect"))) {
                const String effect = json[F("effect")];
                effectsManager->changeEffectByName(effect);
            }
            if (json.containsKey(F("color"))) {
                effectsManager->changeEffectById(F("Color"));
            }
        }
        effectsManager->updateCurrentSettings(json);
        saveLater();
    }

    mqtt->update();
    lampWebServer->update();
}

bool Settings::readSettings()
{
    bool settingsExists = FLASHFS.exists(settingsFileName);
    Serial.printf_P(PSTR("FLASHFS Settings file exists: %s\n"), settingsExists ? PSTR("true") : PSTR("false"));
    if (!settingsExists) {
        saveSettings();
        return false;
    }

    File settings = FLASHFS.open(settingsFileName, "r");
    Serial.printf_P(PSTR("FLASHFS Settings file size: %zu\n"), settings.size());
    if (!settings) {
        Serial.println(F("FLASHFS Error reading settings file"));

        saveSettings();
        return false;
    }

    Serial.println("reading settings.json");
    while (settings.available()) {
        String buffer = settings.readStringUntil('\n');
        Serial.println(buffer);
    }
    settings.seek(0);

    DynamicJsonDocument json(1024);
    DeserializationError err = deserializeJson(json, settings);
    settings.close();
    if (err) {
        Serial.print(F("FLASHFS Error parsing settings json file: "));
        Serial.println(err.c_str());

        saveSettings();
        return false;
    }

    JsonObject root = json.as<JsonObject>();
    if (root.size() == 0) {
        saveSettings();
        return false;
    }

    if (root.containsKey(F("matrix"))) {
       JsonObject matrixObject = root[F("matrix")];
       if (matrixObject.containsKey(F("pin"))) {
           matrixSettings.pin = matrixObject[F("pin")];
       }
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
       if (connectionObject.containsKey(F("mdns"))) {
           connectionSettings.mdns = connectionObject[F("mdns")].as<String>();
       }
       if (connectionObject.containsKey(F("apName"))) {
           connectionSettings.apName = connectionObject[F("apName")].as<String>();
       }
       if (connectionObject.containsKey(F("apPassword"))) {
           connectionSettings.apPassword = connectionObject[F("apPassword")].as<String>();
       }
       if (connectionObject.containsKey(F("ntpServer"))) {
           connectionSettings.ntpServer = connectionObject[F("ntpServer")].as<String>();
       }
       if (connectionObject.containsKey(F("ntpOffset"))) {
           connectionSettings.ntpOffset = connectionObject[F("ntpOffset")];
       }
       if (connectionObject.containsKey(F("hostname"))) {
           connectionSettings.hostname = connectionObject[F("hostname")].as<String>();
       }
       if (connectionObject.containsKey(F("ssid"))) {
           connectionSettings.ssid = connectionObject[F("ssid")].as<String>();
       }
       if (connectionObject.containsKey(F("bssid"))) {
           connectionSettings.bssid = connectionObject[F("bssid")].as<String>();
       }
       if (connectionObject.containsKey(F("password"))) {
           connectionSettings.password = connectionObject[F("password")].as<String>();
       }
       if (connectionObject.containsKey(F("login"))) {
           connectionSettings.login = connectionObject[F("login")].as<String>();
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
       if (mqttObject.containsKey(F("uniqueId"))) {
           mqttSettings.uniqueId = mqttObject[F("uniqueId")].as<String>();
       }
       if (mqttObject.containsKey(F("name"))) {
           mqttSettings.name = mqttObject[F("name")].as<String>();
       }
       if (mqttObject.containsKey(F("model"))) {
           mqttSettings.model = mqttObject[F("model")].as<String>();
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
           buttonSettings.pin = btnPin;
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
    return true;
}

bool Settings::readEffects()
{
    bool effectsExists = FLASHFS.exists(effectsFileName);
    Serial.printf_P(PSTR("FLASHFS Effects file exists: %s\n"), effectsExists ? PSTR("true") : PSTR("false"));
    if (!effectsExists) {
        effectsManager->processAllEffects();
        saveEffects();
        return false;
    }

    File effects = FLASHFS.open(effectsFileName, "r");
    Serial.printf_P(PSTR("FLASHFS Effects file size: %zu\n"), effects.size());
    if (!effects) {
        Serial.println(F("FLASHFS Error reading effects file"));

        effectsManager->processAllEffects();
        saveEffects();
        return false;
    }

    Serial.println("reading effects.json");
    while (effects.available()) {
        String buffer = effects.readStringUntil('\n');
        Serial.println(buffer);
    }
    effects.seek(0);

    DynamicJsonDocument json(serializeSize);
    DeserializationError err = deserializeJson(json, effects);
    effects.close();
    if (err) {
        Serial.print(F("FLASHFS Error parsing effects json file: "));
        Serial.println(err.c_str());

        effectsManager->processAllEffects();
        saveEffects();
        return false;
    }

    JsonArray root = json.as<JsonArray>();
    if (root.size() == 0) {
        effectsManager->processAllEffects();
        saveEffects();
        return false;
    }

    for (JsonObject effect : root) {
        effectsManager->processEffectSettings(effect);
    }
    return true;
}

void Settings::buildSettingsJson(JsonObject &root)
{
    root[F("activeEffect")] = effectsManager->activeEffectIndex();
    root[F("logInterval")] = generalSettings.logInterval;
    root[F("working")] = generalSettings.working;

    JsonObject matrixObject = root.createNestedObject(F("matrix"));
    matrixObject[F("pin")] = matrixSettings.pin;
    matrixObject[F("width")] = matrixSettings.width;
    matrixObject[F("height")] = matrixSettings.height;
    matrixObject[F("segments")] = matrixSettings.segments;
    matrixObject[F("type")] = matrixSettings.type;
    matrixObject[F("maxBrightness")] = matrixSettings.maxBrightness;
    matrixObject[F("currentLimit")] = matrixSettings.currentLimit;
    matrixObject[F("rotation")] = matrixSettings.rotation;

    JsonObject connectionObject = root.createNestedObject(F("connection"));
    connectionObject[F("mdns")] = connectionSettings.mdns;
    connectionObject[F("apName")] = connectionSettings.apName;
    connectionObject[F("apPassword")] = connectionSettings.apPassword;
    connectionObject[F("ntpServer")] = connectionSettings.ntpServer;
    connectionObject[F("ntpOffset")] = connectionSettings.ntpOffset;
    connectionObject[F("hostname")] = connectionSettings.hostname;
    connectionObject[F("ssid")] = connectionSettings.ssid;
    connectionObject[F("bssid")] = connectionSettings.bssid;
    connectionObject[F("password")] = connectionSettings.password;
    connectionObject[F("login")] = connectionSettings.login;

    JsonObject mqttObject = root.createNestedObject(F("mqtt"));
    mqttObject[F("host")] = mqttSettings.host;
    mqttObject[F("port")] = mqttSettings.port;
    mqttObject[F("username")] = mqttSettings.username;
    mqttObject[F("password")] = mqttSettings.password;
    mqttObject[F("uniqueId")] = mqttSettings.uniqueId;
    mqttObject[F("model")] = mqttSettings.model;
    mqttObject[F("name")] = mqttSettings.name;

    JsonObject buttonObject = root.createNestedObject(F("button"));
    buttonObject[F("pin")] = buttonSettings.pin;
    buttonObject[F("type")] = buttonSettings.type;
    buttonObject[F("state")] = buttonSettings.state;

    JsonObject spectrometerObject = root.createNestedObject(F("spectrometer"));
    spectrometerObject[F("active")] = generalSettings.soundControl;
}

void Settings::buildEffectsJson(JsonArray &effects)
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

void Settings::buildJsonMqtt(JsonObject &root)
{
    root[F("state")] = generalSettings.working ? F("ON") : F("OFF");
    root[F("brightness")] = effectsManager->activeEffect()->settings.brightness;
    root[F("speed")] = effectsManager->activeEffect()->settings.speed;
    root[F("scale")] = effectsManager->activeEffect()->settings.scale;
    root[F("effect")] = effectsManager->activeEffect()->settings.name;
    root[F("localIp")] = WiFi.localIP().toString();
#if defined(ESP8266)
    root[F("device")] = F("esp8266");
#else
    root[F("device")] = F("esp32");
#endif
    effectsManager->activeEffect()->writeSettings(root);
}

Settings::Settings(uint32_t saveInterval)
{
    settingsSaveInterval = saveInterval;

    connectionSettings.mdns = F("firelamp");
    connectionSettings.apName = F("Fire Lamp");
    connectionSettings.ntpServer = F("europe.pool.ntp.org");
    connectionSettings.hostname = F("firelamp");

    mqttSettings.uniqueId = GetUniqueID();
    mqttSettings.manufacturer = F("coderus");
}
