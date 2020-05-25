#pragma once
#include "effects/Effect.h"

class SpiralEffect : public Effect
{
public:
    explicit SpiralEffect(const String &id);
    void activate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

