#pragma once
#include "effects/Effect.h"

class StormEffect : public Effect
{
public:
    StormEffect();
    void tick() override;
    void writeSettings(JsonObject &json) override;
    void initialize(const JsonObject &json) override;
};

