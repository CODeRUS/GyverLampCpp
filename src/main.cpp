#include <Arduino.h>

#if defined(ESP32)
#include <SPIFFS.h>
#else
#include <FS.h>
#endif

#include "LocalDNS.h"
#include "MyMatrix.h"
#include "EffectsManager.h"
#include "Settings.h"
#include "GyverTimer.h"

#include "GyverButton.h"
#include "LampWebServer.h"

#include "effects/Effect.h"

namespace  {

uint16_t webServerPort = 80;
uint16_t webSocketPort = 8000;

#if defined(ESP32)
const uint8_t btnPin = 15;
#elif defined(SONOFF)
const uint8_t btnPin = 5;
#else
const uint8_t btnPin = D5;
#endif

GButton *button = nullptr;

int stepDirection = 1;
bool isHolding = false;

uint32_t logTimer = 0;
uint32_t logInterval = 10 * 1000;

void printFreeHeap()
{
    Serial.print(F("FreeHeap: "));
    Serial.println(ESP.getFreeHeap());
}

void processButton()
{
    button->tick();
    if (button->isSingle()) {
        Serial.println(F("Single button"));
        effectsManager->working = !effectsManager->working;
        if (!effectsManager->working) {
            myMatrix->clear(true);
        }
    }
    if (!effectsManager->working) {
        return;
    }
    if (button->isDouble()) {
        Serial.println(F("Double button"));
        effectsManager->Next();
        mySettings->SaveLater();
    }
    if (button->isTriple()) {
        Serial.println(F("Triple button"));
        effectsManager->Previous();
        mySettings->SaveLater();
    }
    if (button->isHolded()) {
        Serial.println(F("Holded button"));
        isHolding = true;
        const uint8_t brightness = effectsManager->activeEffect()->brightness();
        if (brightness <= 1) {
            stepDirection = 1;
        } else if (brightness == 255) {
            stepDirection = -1;
        }
    }
    if (isHolding && button->isStep()) {
        uint8_t brightness = effectsManager->activeEffect()->brightness();
        if (stepDirection < 0 && brightness == 1) {
            return;
        }
        if (stepDirection > 0 && brightness == 255) {
            return;
        }
        brightness += stepDirection;
        Serial.printf_P(PSTR("Step button %d. brightness: %u\n"), stepDirection, brightness);
        effectsManager->activeEffect()->setBrightness(brightness);
        myMatrix->setBrightness(brightness);
    }
    if (button->isRelease() && isHolding) {
        Serial.println(F("Release button"));
        mySettings->SaveLater();
        isHolding = false;
    }
}

void setupSerial()
{
    Serial.begin(115200);
    Serial.println(F("\nHappy debugging!"));
    Serial.flush();
}

}

void setup() {
#if defined(ESP8266)
    ESP.wdtDisable();
    ESP.wdtEnable(0);
#endif

    setupSerial();
    printFreeHeap();

    if(!SPIFFS.begin()) {
        Serial.println(F("An Error has occurred while mounting SPIFFS"));
        return;
    }
    Settings::Initialize();

    MyMatrix::Initialize();
    myMatrix->matrixTest();

    EffectsManager::Initialize();
    LampWebServer::Initialize(webServerPort, webSocketPort);

    Serial.println(F("AutoConnect started"));
    lampWebServer->AutoConnect();
    Serial.println(F("AutoConnect finished"));
    if (LocalDNS::Begin()) {
        LocalDNS::AddService(PSTR("http"), PSTR("tcp"), webServerPort);
        LocalDNS::AddService(PSTR("ws"), PSTR("tcp"), webSocketPort);
    } else {
        Serial.println(F("An Error has occurred while initializing mDNS"));
    }
    lampWebServer->StartServer();
    if (lampWebServer->IsConnected()) {
        GyverTimer::Initialize();
    }

    button = new GButton(btnPin, GButton::PullTypeLow, GButton::DefaultStateOpen);
    button->setTickMode(false);
    button->setStepTimeout(20);
}

void loop() {
#if defined(ESP8266)
    ESP.wdtFeed();
#endif

    lampWebServer->Process();

    if (lampWebServer->isUpdating()) {
        return;
    }

    GyverTimer::Process();
    processButton();

    if (effectsManager->working) {
        effectsManager->Process();
    }

    mySettings->Process();

    if (millis() - logTimer > logInterval) {
        printFreeHeap();
        logTimer = millis();
    }
}
