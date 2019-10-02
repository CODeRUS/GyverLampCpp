#pragma once
#include "effects/Effect.h"

class StarfallEffect : public Effect
{
public:
    StarfallEffect();
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

