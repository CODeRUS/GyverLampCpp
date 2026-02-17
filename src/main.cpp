#include <Arduino.h>

#if defined(ESP32)
#include <esp_wifi.h>
#include <WiFi.h>
#include <SPIFFS.h>
#define FLASHFS SPIFFS
#else
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#define FLASHFS LittleFS
#endif

#include <EEPROM.h>

#include "LocalDNS.h"
#include "MyMatrix.h"
#include "EffectsManager.h"
#include "Settings.h"
#include "TimeClient.h"

#include "GyverButton.h"
#include "LampWebServer.h"

#include "effects/Effect.h"

#include "Spectrometer.h"
#include "MqttClient.h"
#include <cstring>

namespace  {

uint16_t webServerPort = 80;

#if defined(ESP32)
const uint8_t btnPin = 15;
const GButton::PullType btnType = GButton::PullTypeLow;
#elif defined(SONOFF)
const uint8_t btnPin = 0;
const GButton::PullType btnType = GButton::PullTypeHigh;
const uint8_t relayPin = 12;
const uint8_t miniLedPin = 13;
#else
const uint8_t btnPin = 4;
const GButton::PullType btnType = GButton::PullTypeLow;
#endif
const GButton::DefaultState btnState = GButton::DefaultStateOpen;

GButton *button = nullptr;

int stepDirection = 1;
bool isHolding = false;

uint32_t logTimer = 0;

bool setupMode = false;
bool connectFinished = false;
uint32_t serialWifiRestartTimer = 0;
uint32_t improvStateBroadcastTimer = 0;
bool settingsReadyForImprov = false;
bool flashFsReadyForLogs = false;
bool improvProvisionPending = false;
bool improvProvisionSettingsSaved = false;
uint8_t improvProvisionCommand = 0x01;
uint32_t improvProvisionStartedAt = 0;
String improvPendingSsid;
String improvPendingPassword;
constexpr bool IMPROV_DEBUG_FIXED_SCAN_RESPONSE = false;

constexpr uint8_t IMPROV_HEADER_SIZE = 6;
constexpr uint8_t IMPROV_MIN_PACKET_SIZE = IMPROV_HEADER_SIZE + 4;
constexpr char IMPROV_HEADER[IMPROV_HEADER_SIZE] = {'I', 'M', 'P', 'R', 'O', 'V'};
constexpr uint8_t IMPROV_PROTOCOL_VERSION = 1;

enum ImprovPacketType : uint8_t {
    IMPROV_TYPE_CURRENT_STATE = 0x01,
    IMPROV_TYPE_ERROR_STATE = 0x02,
    IMPROV_TYPE_RPC = 0x03,
    IMPROV_TYPE_RPC_RESPONSE = 0x04
};

enum ImprovState : uint8_t {
    IMPROV_STATE_STOPPED = 0x00,
    IMPROV_STATE_AWAITING_AUTHORIZATION = 0x01,
    IMPROV_STATE_AUTHORIZED = 0x02,
    IMPROV_STATE_PROVISIONING = 0x03,
    IMPROV_STATE_PROVISIONED = 0x04
};

enum ImprovError : uint8_t {
    IMPROV_ERROR_NONE = 0x00,
    IMPROV_ERROR_INVALID_RPC = 0x01,
    IMPROV_ERROR_UNKNOWN_RPC = 0x02,
    IMPROV_ERROR_UNABLE_TO_CONNECT = 0x03,
    IMPROV_ERROR_NOT_AUTHORIZED = 0x04,
    IMPROV_ERROR_UNKNOWN = 0xFF
};

enum ImprovCommand : uint8_t {
    IMPROV_CMD_UNKNOWN = 0x00,
    IMPROV_CMD_WIFI_SETTINGS = 0x01,
    IMPROV_CMD_GET_CURRENT_STATE = 0x02,
    IMPROV_CMD_GET_DEVICE_INFO = 0x03,
    IMPROV_CMD_GET_WIFI_NETWORKS = 0x04,
    IMPROV_CMD_IDENTIFY = 0x05
};

struct ImprovRpcData {
    uint8_t rawCommand = 0;
    ImprovCommand command = IMPROV_CMD_UNKNOWN;
    String ssid;
    String password;
    bool hasDataLengthPrefix = false;
    uint8_t dataOffset = 0;
    uint8_t rpcDataLength = 0;
    bool valid = false;
};

uint8_t improvState = IMPROV_STATE_AUTHORIZED;
uint8_t serialProvisioningBuffer[768];
size_t serialProvisioningLength = 0;

uint8_t calcChecksumWithHeader(const uint8_t *data, size_t dataLength)
{
    uint16_t sum = 0;
    for (const char c : IMPROV_HEADER) {
        sum += static_cast<uint8_t>(c);
    }
    for (size_t i = 0; i < dataLength; i++) {
        sum += data[i];
    }
    return static_cast<uint8_t>(sum & 0xFF);
}

uint8_t calcChecksumWithoutHeader(const uint8_t *data, size_t dataLength)
{
    uint16_t sum = 0;
    for (size_t i = 0; i < dataLength; i++) {
        sum += data[i];
    }
    return static_cast<uint8_t>(sum & 0xFF);
}

void flushImprovPreFsLogBuffer()
{
}

void dumpImprovLogIfExists()
{
}

void resetImprovLogIfTooLarge()
{
}

void sendImprovPacket(uint8_t type, const uint8_t *payload, uint8_t payloadLength)
{
    uint8_t frame[IMPROV_MIN_PACKET_SIZE + 255] = {0};
    uint8_t idx = 0;
    for (uint8_t i = 0; i < IMPROV_HEADER_SIZE; i++) {
        frame[idx++] = static_cast<uint8_t>(IMPROV_HEADER[i]);
    }
    frame[idx++] = IMPROV_PROTOCOL_VERSION;
    frame[idx++] = type;
    frame[idx++] = payloadLength;
    for (uint8_t i = 0; i < payloadLength; i++) {
        frame[idx++] = payload[i];
    }

    const uint8_t bodyStart = IMPROV_HEADER_SIZE;
    const size_t bodyLength = static_cast<size_t>(3 + payloadLength);
    frame[idx] = calcChecksumWithHeader(frame + bodyStart, bodyLength);

    Serial.write(frame, idx + 1);
}

void sendImprovState(uint8_t state)
{
    improvState = state;
    const uint8_t payload[] = {state};
    sendImprovPacket(IMPROV_TYPE_CURRENT_STATE, payload, sizeof(payload));
}

void sendImprovError(uint8_t error)
{
    const uint8_t payload[] = {error};
    sendImprovPacket(IMPROV_TYPE_ERROR_STATE, payload, sizeof(payload));
}

bool appendImprovString(uint8_t *payload, uint8_t &payloadLength, const uint8_t maxPayloadLength, const String &value)
{
    const uint8_t valueLength = static_cast<uint8_t>(value.length() > 255 ? 255 : value.length());
    if (payloadLength + 1 + valueLength > maxPayloadLength) {
        return false;
    }
    payload[payloadLength++] = valueLength;
    for (uint8_t i = 0; i < valueLength; i++) {
        payload[payloadLength++] = static_cast<uint8_t>(value[i]);
    }
    return true;
}

void sendImprovRpcResponse(uint8_t command, const String values[], uint8_t valuesCount)
{
    uint8_t payload[255] = {0};
    uint8_t payloadLength = 0;
    uint8_t dataBlock[252] = {0};
    uint8_t dataBlockLength = 0;
    for (uint8_t i = 0; i < valuesCount; i++) {
        if (!appendImprovString(dataBlock, dataBlockLength, sizeof(dataBlock), values[i])) {
            break;
        }
    }
    payload[payloadLength++] = static_cast<uint8_t>(command);
    payload[payloadLength++] = dataBlockLength;
    for (uint8_t i = 0; i < dataBlockLength; i++) {
        payload[payloadLength++] = dataBlock[i];
    }
    sendImprovPacket(IMPROV_TYPE_RPC_RESPONSE, payload, payloadLength);
}

ImprovRpcData parseImprovRpc(const uint8_t *payload, uint8_t payloadLength)
{
    ImprovRpcData data;
    if (payloadLength == 0) {
        return data;
    }

    data.rawCommand = payload[0];
    data.command = static_cast<ImprovCommand>(data.rawCommand);
    data.valid = true;

    if (data.command != IMPROV_CMD_WIFI_SETTINGS) {
        return data;
    }

    uint8_t dataOffset = 1;
    uint8_t rpcDataLength = payloadLength - dataOffset;
    if (payloadLength >= 2) {
        rpcDataLength = payload[1];
        dataOffset = 2;
        data.hasDataLengthPrefix = true;
    }
    data.dataOffset = dataOffset;
    data.rpcDataLength = rpcDataLength;

    if (rpcDataLength == 0 || payloadLength < static_cast<uint8_t>(dataOffset + 2)) {
        data.valid = false;
        return data;
    }

    if (payloadLength < static_cast<uint8_t>(dataOffset + rpcDataLength)) {
        data.valid = false;
        return data;
    }

    const uint8_t ssidLength = payload[dataOffset];
    if (rpcDataLength < static_cast<uint8_t>(ssidLength + 2)) {
        data.valid = false;
        return data;
    }

    data.ssid.reserve(ssidLength);
    for (uint8_t i = 0; i < ssidLength; i++) {
        data.ssid += static_cast<char>(payload[dataOffset + 1 + i]);
    }

    const uint8_t passwordLengthOffset = dataOffset + 1 + ssidLength;
    const uint8_t passwordLength = payload[passwordLengthOffset];
    const uint16_t requiredDataLength = static_cast<uint16_t>(ssidLength + passwordLength + 2);
    if (rpcDataLength < requiredDataLength) {
        data.valid = false;
        return data;
    }

    data.password.reserve(passwordLength);
    for (uint8_t i = 0; i < passwordLength; i++) {
        data.password += static_cast<char>(payload[passwordLengthOffset + 1 + i]);
    }

    return data;
}

String getProvisionedUrl()
{
    if (WiFi.status() == WL_CONNECTED) {
        const IPAddress ip = WiFi.localIP();
        return String(F("http://")) + ip.toString() + F("/");
    }

    const String hostname = (settingsReadyForImprov && mySettings)
        ? mySettings->connectionSettings.hostname
        : String(F("firelamp"));
    if (!hostname.isEmpty()) {
        return String(F("http://")) + hostname + F(".local/");
    }

    return String(F("http://192.168.4.1/"));
}

void sendImprovScanResults(uint8_t responseCommand)
{
    if (IMPROV_DEBUG_FIXED_SCAN_RESPONSE) {
        const String values[] = {String(F("TestNet")), String(F("-50")), String(F("NO"))};
        sendImprovRpcResponse(responseCommand, values, 3);
        sendImprovRpcResponse(responseCommand, nullptr, 0);
        return;
    }

    int total = WiFi.scanComplete();
    if (total == WIFI_SCAN_FAILED) {
        total = WiFi.scanNetworks(false, true);
    } else if (total == WIFI_SCAN_RUNNING) {
        const uint32_t startedAt = millis();
        while ((total = WiFi.scanComplete()) == WIFI_SCAN_RUNNING && millis() - startedAt < 7000) {
#if defined(ESP8266)
            ESP.wdtFeed();
#endif
            delay(20);
        }
        if (total <= 0) {
            total = WiFi.scanNetworks(false, true);
        }
    }

    struct ScanEntry {
        int index;
        int32_t rssi;
    };
    ScanEntry entries[64];
    uint8_t entriesCount = 0;
    for (int i = 0; i < total && entriesCount < 64; i++) {
        String ssid = WiFi.SSID(i);
        ssid.trim();
        if (ssid.isEmpty()) {
            continue;
        }
        entries[entriesCount].index = i;
        entries[entriesCount].rssi = WiFi.RSSI(i);
        entriesCount++;
    }

    // Sort stronger signal first to improve UX in the provisioning dialog.
    for (uint8_t i = 0; i < entriesCount; i++) {
        uint8_t best = i;
        for (uint8_t j = static_cast<uint8_t>(i + 1); j < entriesCount; j++) {
            if (entries[j].rssi > entries[best].rssi) {
                best = j;
            }
        }
        if (best != i) {
            const ScanEntry tmp = entries[i];
            entries[i] = entries[best];
            entries[best] = tmp;
        }
    }

    uint8_t sent = 0;
    for (uint8_t i = 0; i < entriesCount && sent < 20; i++) {
        const int networkIndex = entries[i].index;
        String ssid = WiFi.SSID(networkIndex);
        ssid.trim();
        if (ssid.isEmpty()) {
            continue;
        }

#if defined(ESP8266)
        const bool isOpen = WiFi.encryptionType(networkIndex) == ENC_TYPE_NONE;
#else
        const bool isOpen = WiFi.encryptionType(networkIndex) == WIFI_AUTH_OPEN;
#endif
        const String rssi = String(entries[i].rssi);
        const String secure = isOpen ? String(F("NO")) : String(F("YES"));
        const String values[] = {ssid, rssi, secure};
        sendImprovRpcResponse(responseCommand, values, 3);
        sent++;
    }

    // End-of-list marker expected by clients (notably esp-web-tools/WLED flow).
    sendImprovRpcResponse(responseCommand, nullptr, 0);

#if defined(ESP8266)
    WiFi.scanDelete();
#endif
    WiFi.scanNetworks(true, true);
}

void sendImprovDeviceInfoForCommand(uint8_t rawCommand)
{
    const String hostname = (settingsReadyForImprov && mySettings)
        ? mySettings->connectionSettings.hostname
        : String(F("firelamp"));
    const String values[] = {
        String(F("GyverLampCpp")),
        String(F("improv-serial")),
#if defined(ESP32)
        String(F("ESP32")),
#else
        String(F("ESP8266")),
#endif
        hostname
    };
    sendImprovRpcResponse(rawCommand, values, 4);
}

void handleImprovRpc(const uint8_t *payload, uint8_t payloadLength)
{
    const ImprovRpcData rpc = parseImprovRpc(payload, payloadLength);
    if (!rpc.valid) {
        sendImprovError(IMPROV_ERROR_INVALID_RPC);
        return;
    }

    // Compatibility path for esp-web-tools variants that use 0x02/0x03/0x04 sequence.
    if (payloadLength <= 2) {
        if (rpc.rawCommand == 0x02) {
            sendImprovState(improvState);
            return;
        }
        if (rpc.rawCommand == 0x03) {
            sendImprovDeviceInfoForCommand(rpc.rawCommand);
            return;
        }
        if (rpc.rawCommand == 0x04) {
            sendImprovScanResults(rpc.rawCommand);
            return;
        }
        if (rpc.rawCommand == 0x05) {
            sendImprovScanResults(rpc.rawCommand);
            return;
        }
    }

    if (rpc.command == IMPROV_CMD_GET_CURRENT_STATE) {
        sendImprovState(improvState);
        return;
    }

    if (rpc.command == IMPROV_CMD_GET_DEVICE_INFO) {
        sendImprovDeviceInfoForCommand(rpc.rawCommand);
        return;
    }

    if (rpc.command == IMPROV_CMD_IDENTIFY) {
        // Do not respond with RPC result here to avoid response ordering issues.
        return;
    }

    if (rpc.command == IMPROV_CMD_GET_WIFI_NETWORKS) {
        sendImprovScanResults(rpc.rawCommand);
        return;
    }

    if (rpc.command != IMPROV_CMD_WIFI_SETTINGS) {
        sendImprovError(IMPROV_ERROR_UNKNOWN_RPC);
        return;
    }

    if (rpc.ssid.isEmpty()) {
        sendImprovError(IMPROV_ERROR_INVALID_RPC);
        return;
    }

    sendImprovState(IMPROV_STATE_PROVISIONING);
    improvPendingSsid = rpc.ssid;
    improvPendingPassword = rpc.password;
    improvProvisionSettingsSaved = false;

    if (settingsReadyForImprov && mySettings) {
        mySettings->connectionSettings.ssid = improvPendingSsid;
        mySettings->connectionSettings.password = improvPendingPassword;
        mySettings->connectionSettings.bssid = String();
        mySettings->saveSettings();
        improvProvisionSettingsSaved = true;
    }
    improvProvisionPending = true;
    improvProvisionCommand = rpc.rawCommand;
    improvProvisionStartedAt = millis();

    // Match WLED behavior: do not send PROVISIONED/URL until Wi-Fi is actually connected.
    WiFi.disconnect();
    delay(100);
    WiFi.begin(rpc.ssid.c_str(), rpc.password.c_str());
}

bool verifyIncomingImprovChecksum(const uint8_t *packet, size_t packetLength)
{
    if (packetLength < IMPROV_MIN_PACKET_SIZE) {
        return false;
    }

    const uint8_t payloadLength = packet[8];
    const size_t bodyLength = static_cast<size_t>(3 + payloadLength);
    const uint8_t expected = packet[9 + payloadLength];
    const uint8_t checksumWithoutHeader = calcChecksumWithoutHeader(packet + 6, bodyLength);
    const uint8_t checksumWithHeader = calcChecksumWithHeader(packet + 6, bodyLength);
    return expected == checksumWithoutHeader || expected == checksumWithHeader;
}

void processSerialProvisioning()
{
    while (Serial.available()) {
        if (serialProvisioningLength >= sizeof(serialProvisioningBuffer)) {
            serialProvisioningLength = 0;
        }
        serialProvisioningBuffer[serialProvisioningLength++] = static_cast<uint8_t>(Serial.read());
    }

    while (serialProvisioningLength >= IMPROV_MIN_PACKET_SIZE) {
        size_t headerOffset = 0;
        bool foundHeader = false;
        while (headerOffset + IMPROV_HEADER_SIZE <= serialProvisioningLength) {
            if (memcmp(serialProvisioningBuffer + headerOffset, IMPROV_HEADER, IMPROV_HEADER_SIZE) == 0) {
                foundHeader = true;
                break;
            }
            headerOffset++;
        }

        if (!foundHeader) {
            serialProvisioningLength = 0;
            return;
        }

        if (headerOffset > 0) {
            memmove(serialProvisioningBuffer, serialProvisioningBuffer + headerOffset, serialProvisioningLength - headerOffset);
            serialProvisioningLength -= headerOffset;
            if (serialProvisioningLength < IMPROV_MIN_PACKET_SIZE) {
                return;
            }
        }

        const uint8_t payloadLength = serialProvisioningBuffer[8];
        const size_t packetLength = static_cast<size_t>(IMPROV_HEADER_SIZE + 4 + payloadLength);
        if (serialProvisioningLength < packetLength) {
            return;
        }

        const bool validChecksum = verifyIncomingImprovChecksum(serialProvisioningBuffer, packetLength);
        if (validChecksum && serialProvisioningBuffer[7] == IMPROV_TYPE_RPC) {
            handleImprovRpc(serialProvisioningBuffer + 9, payloadLength);
        }

        if (serialProvisioningLength > packetLength) {
            memmove(serialProvisioningBuffer, serialProvisioningBuffer + packetLength, serialProvisioningLength - packetLength);
        }
        serialProvisioningLength -= packetLength;
    }
}

void serviceImprov()
{
    processSerialProvisioning();
}

void runImprovBootstrapWindow(uint32_t durationMs)
{
    const uint32_t startedAt = millis();
    while (millis() - startedAt < durationMs) {
        serviceImprov();
#if defined(ESP8266)
        ESP.wdtFeed();
#endif
        delay(10);
    }
}

void processMatrix()
{
    if (mySettings->generalSettings.working) {
        effectsManager->loop();
    } else {
        myMatrix->clear(true);
    }
}

void printFlashInfo()
{
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();

    Serial.printf_P(PSTR("Flash ide  size: %u bytes\n"), ideSize);
    Serial.printf_P(PSTR("Flash ide speed: %u Hz\n"), ESP.getFlashChipSpeed());
    Serial.print(F("Flash ide mode:  "));
    Serial.println((ideMode == FM_QIO ? F("QIO") : ideMode == FM_QOUT ? F("QOUT") : ideMode == FM_DIO ? F("DIO") : ideMode == FM_DOUT ? F("DOUT") : F("UNKNOWN")));

#if defined(ESP8266)
    uint32_t realSize = ESP.getFlashChipRealSize();
    Serial.printf_P(PSTR("Flash real id:   %08X\n"), ESP.getFlashChipId());
    Serial.printf_P(PSTR("Flash real size: %u bytes\n\n"), realSize);
    if (ideSize != realSize) {
      Serial.println(F("Flash Chip configuration wrong!"));
    } else {
      Serial.println(F("Flash Chip configuration ok."));
    }
#endif

    Serial.print(F("Sketch size: "));
    Serial.println(ESP.getSketchSize());
    Serial.print(F("Sketch free: "));
    Serial.println(ESP.getFreeSketchSpace());

#if defined(ESP32)
    Serial.print(F("Total heap: "));
    Serial.println(ESP.getHeapSize());
    Serial.print(F("Min free heap: "));
    Serial.println(ESP.getMinFreeHeap());
    Serial.print(F("Max alloc heap: "));
    Serial.println(ESP.getMaxAllocHeap());
#endif
}

void printFreeHeap()
{
    static uint32_t s_counter = 0;
    Serial.print(++s_counter);
    Serial.print(F("_FreeHeap: "));
    Serial.println(ESP.getFreeHeap());
//    Serial.flush();
}

void processButton()
{
    if (mySettings->buttonSettings.pin == 255) {
        return;
    }
    button->tick();
    if (button->isSingle() && !mySettings->busy) {
        Serial.println(F("Single button"));
        mySettings->generalSettings.working = !mySettings->generalSettings.working;
        mySettings->saveLater();
        if (mqtt) {
            mqtt->update();
        }
        if (lampWebServer) {
            lampWebServer->update();
        }
    }
    if (!mySettings->generalSettings.working) {
        return;
    }
    if (button->isDouble() && !mySettings->busy) {
        Serial.println(F("Double button"));
        effectsManager->next();
    }
    if (button->isTriple() && !mySettings->busy) {
        Serial.println(F("Triple button"));
        effectsManager->previous();
    }
    if (button->isHolded() && !mySettings->busy) {
        Serial.println(F("Holded button"));
        isHolding = true;
        const uint8_t brightness = effectsManager->activeEffect()->settings.brightness;
        if (brightness <= 1) {
            stepDirection = 1;
        } else if (brightness == 255) {
            stepDirection = -1;
        }
        mySettings->saveLater();
        if (mqtt) {
            mqtt->update();
        }
        if (lampWebServer) {
            lampWebServer->update();
        }
    }
    if (isHolding && button->isStep()) {
        uint8_t brightness = effectsManager->activeEffect()->settings.brightness;
        if (stepDirection < 0 && brightness == 1) {
            return;
        }
        if (stepDirection > 0 && brightness == 255) {
            return;
        }
        brightness += stepDirection;
        Serial.printf_P(PSTR("Step button %d. brightness: %u\n"), stepDirection, brightness);
        effectsManager->activeEffect()->settings.brightness = brightness;
        myMatrix->setBrightness(brightness);
        mySettings->saveLater();
        if (mqtt) {
            mqtt->update();
        }
        if (lampWebServer) {
            lampWebServer->update();
        }
    }
    if (button->isRelease() && isHolding) {
        Serial.println(F("Release button"));
        isHolding = false;
    }
}

void setupSerial()
{
    Serial.begin(115200);
    // Serial.println(F("\nHappy debugging!"));
    // Serial.flush();
}

}

void clearWifi()
{
#if defined(ESP32)
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    delay(2000);
    esp_wifi_restore();
#else
    WiFi.disconnect();
#endif
}

void setup() {
#if defined(ESP8266)
    ESP.wdtDisable();
    ESP.wdtEnable(0);
#endif

    // Keep boot responsive for esp-web-tools Improv detection after flashing.
    delay(250);

    setupSerial();
    sendImprovState(IMPROV_STATE_AUTHORIZED);
    improvStateBroadcastTimer = millis();
    // Give esp-web-tools enough time to detect Improv before heavy startup.
    runImprovBootstrapWindow(12000);
    serviceImprov();

    bool flashFsMounted = FLASHFS.begin();
#if defined(ESP32)
    if (!flashFsMounted) {
        Serial.println(F("FLASHFS mount failed, trying format+mount..."));
        flashFsMounted = FLASHFS.begin(true);
    }
#endif
    if (!flashFsMounted) {
        Serial.println(F("An Error has occurred while mounting FLASHFS"));
        flashFsReadyForLogs = false;
    } else {
        flashFsReadyForLogs = true;
        resetImprovLogIfTooLarge();
        flushImprovPreFsLogBuffer();
        dumpImprovLogIfExists();
    }
    serviceImprov();

    Settings::Initialize();
// default values for button
    mySettings->buttonSettings.pin = btnPin;
    mySettings->buttonSettings.type = btnType;
    mySettings->buttonSettings.state = btnState;
    if (!mySettings->readSettings()) {
        mySettings->buttonSettings.pin = 255;
    }
    settingsReadyForImprov = true;
    serviceImprov();

    Serial.printf_P(PSTR("Button pin: %d\n"), mySettings->buttonSettings.pin);

    EffectsManager::Initialize();
    serviceImprov();
    mySettings->readEffects();
    serviceImprov();
    MyMatrix::Initialize();
    serviceImprov();

#if defined(SONOFF)
    pinMode(relayPin, OUTPUT);
    pinMode(miniLedPin, OUTPUT);
#endif

    button = new GButton(mySettings->buttonSettings.pin,
                         mySettings->buttonSettings.type,
                         mySettings->buttonSettings.state);
    button->setTickMode(false);
    button->setStepTimeout(20);

    if (mySettings->generalSettings.working) {
        myMatrix->matrixTest();
    }

    button->tick();
    if (button->state()) {
        Serial.println(F("!!! Setup mode entered. No effects !!!"));
        myMatrix->setBrightness(80);
        myMatrix->fill(CRGB(0, 20, 0), true);
        setupMode = true;
        myMatrix->show();
        clearWifi();
    }

    LampWebServer::Initialize(webServerPort);
    serviceImprov();
    if (setupMode) {
        lampWebServer->enterSetupMode();
    }

    Serial.println(F("AutoConnect started"));
    lampWebServer->onConnected([](bool isConnected) {
        if (connectFinished) {
            return;
        }
        Serial.println(F("AutoConnect finished"));
        if (isConnected) {
            LocalDNS::Initialize();
            if (localDNS->begin()) {
                localDNS->addService(F("http"), F("tcp"), webServerPort);
            } else {
                Serial.println(F("An Error has occurred while initializing mDNS"));
            }
            if (!setupMode) {
                TimeClient::Initialize();
                MqttClient::Initialize();
            }
        }

//    if (mySettings->generalSettings.soundControl) {
//        Spectrometer::Initialize();
//    }
        if (!setupMode) {
            effectsManager->activateEffect(mySettings->generalSettings.activeEffect, false);
        }
        connectFinished = true;
    });
    lampWebServer->autoConnect();
    serviceImprov();
}

void loop() {
#if defined(ESP8266)
    ESP.wdtFeed();
#endif

    serviceImprov();
    if (!mySettings || !lampWebServer) {
        delay(10);
        return;
    }
    if (improvProvisionPending) {
        if (!improvProvisionSettingsSaved && settingsReadyForImprov && mySettings && !improvPendingSsid.isEmpty()) {
            mySettings->connectionSettings.ssid = improvPendingSsid;
            mySettings->connectionSettings.password = improvPendingPassword;
            mySettings->connectionSettings.bssid = String();
            mySettings->saveSettings();
            improvProvisionSettingsSaved = true;
        }
        if (WiFi.status() == WL_CONNECTED) {
            const String values[] = {getProvisionedUrl()};
            sendImprovRpcResponse(improvProvisionCommand, values, 1);
            sendImprovState(IMPROV_STATE_PROVISIONED);
            improvProvisionPending = false;
            improvPendingSsid = String();
            improvPendingPassword = String();
        } else if (millis() - improvProvisionStartedAt > 30000) {
            sendImprovError(IMPROV_ERROR_UNABLE_TO_CONNECT);
            improvProvisionPending = false;
            improvPendingSsid = String();
            improvPendingPassword = String();
        }
    }
    if (serialWifiRestartTimer > 0 && millis() > serialWifiRestartTimer) {
        ESP.restart();
    }

    if (mySettings->generalSettings.logInterval > 0 && millis() - logTimer > mySettings->generalSettings.logInterval) {
        printFreeHeap();
        logTimer = millis();
    }

    lampWebServer->loop();

    if (!connectFinished) {
        return;
    }

    if (lampWebServer->isUpdating()) {
        return;
    }

    localDNS->loop();

    if (setupMode) {
        return;
    }

    if (lampWebServer->isConnected()) {
        timeClient->loop();
    }
    processButton();
#if defined(SONOFF)
    digitalWrite(relayPin, mySettings->generalSettings.working);
    digitalWrite(miniLedPin, mySettings->generalSettings.working);
#endif

//    if (mySettings->generalSettings.soundControl) {
//        mySpectrometer->loop();
//    }

    processMatrix();
    mySettings->loop();
}
