#pragma once
#include "effects/Effect.h"

class SnowEffect : public Effect
{
public:
    SnowEffect();
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

