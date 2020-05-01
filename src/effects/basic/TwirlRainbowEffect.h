#pragma once
#include "effects/Effect.h"

class TwirlRainbowEffect : public Effect
{
public:
    TwirlRainbowEffect();
    void activate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};
