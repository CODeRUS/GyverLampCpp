#pragma once
#include "effects/Effect.h"

class SpiralEffect : public Effect
{
public:
    SpiralEffect();
    void activate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

