#pragma once
#include "MyMatrix.h"
#include "Settings.h"
#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <ArduinoJson.h>

class Effect
{
public:
    struct EffectSettings {
        String name = "";
        uint8_t speed = 1;
        uint8_t scale = 100;
        uint8_t brightness = 80;
    };

    Effect();
    virtual ~Effect();
    void Process();

    virtual void activate() {}
    virtual void deactivate() {}

    virtual void initialize(const JsonObject &json);
    virtual void update(const JsonObject &json);

    virtual void tick() = 0;

    EffectSettings settings;

    String name();
    uint8_t speed();
    uint8_t scale();
    uint8_t brightness();

    void setBrightness(uint8_t newBrightness);

protected:
    uint8_t width = 0;
    uint8_t height = 0;
};
