#include "Settings.h"
#include "EffectsManager.h"

uint8_t Settings::initializationFlag = 0;
Settings::AlarmSettings* Settings::alarmSettings;
uint8_t Settings::currentEffect = 0;
Settings::EffectSettings* Settings::effectsSettings;
uint8_t Settings::dawnMode = 0;
bool Settings::masterSwitch = true;

static const uint8_t alarmsCount = 7;
static bool settingsChanged = false;
static uint32_t eepromTimer = 0;
static uint32_t eepromSaveInterval = 3000;

void Settings::Initialize(const uint8_t eepromInitialization, uint32_t saveInterval)
{
    eepromSaveInterval = saveInterval;

    alarmSettings = new AlarmSettings[alarmsCount]();
    effectsSettings = new EffectSettings[EffectsManager::Count()]();

    const bool eepromReady =
            EEPROM.begin(sizeof(uint8_t) // initializationFlag
                         + sizeof(AlarmSettings) * alarmsCount // alarmSettings
                         + sizeof(uint8_t) // currentEffect
                         + sizeof(EffectSettings) * EffectsManager::Count() // effectsSettings
                         + sizeof(uint8_t)); // dawnMode
    Serial.printf("EEPROM ready: %s\n", eepromReady ? "true" : "false");

    int address = 0;

    initializationFlag = EEPROM.readByte(address);
    Serial.printf("initialization read: %u, expected: %u\n", initializationFlag, eepromInitialization);
    if (eepromInitialization != initializationFlag) {
        initializationFlag = eepromInitialization;
        Serial.println("Erasing EEPROM");
        Save();
    }

    address += sizeof(uint8_t);

    for (int i = 0; i < alarmsCount; i++) {
        EEPROM.get(address, alarmSettings[i]);
        address += sizeof(AlarmSettings);
    }

    currentEffect = EEPROM.readByte(address);
    address += sizeof(uint8_t);

    for (uint8_t i = 0; i < EffectsManager::Count(); i++) {
        EEPROM.get(address, effectsSettings[i]);
        Serial.printf("Read effect %s, speed: %u, scale: %u, brightness: %u\n",
                      EffectsManager::EffectName(i).c_str(),
                      effectsSettings[i].effectSpeed,
                      effectsSettings[i].effectScale,
                      effectsSettings[i].effectBrightness);
        address += sizeof(EffectSettings);
    }

    dawnMode = EEPROM.readByte(address);
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

    EEPROM.writeByte(address, initializationFlag);
    address += sizeof(uint8_t);

    for (int i = 0; i < alarmsCount; i++) {
        EEPROM.put(address, alarmSettings[i]);
        address += sizeof(AlarmSettings);
    }

    EEPROM.writeByte(address, currentEffect);
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

    EEPROM.writeByte(address, dawnMode);
    address += sizeof(uint8_t);

    EEPROM.commit();
}

Settings::EffectSettings* Settings::CurrentEffectSettings()
{
    return &effectsSettings[currentEffect];
}
