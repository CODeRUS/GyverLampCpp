#pragma once
#include "effects/Effect.h"

class TwirlRainbowEffect : public Effect
{
public:
    explicit TwirlRainbowEffect(const String &id);
    void activate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};
