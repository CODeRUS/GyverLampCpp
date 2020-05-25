#pragma once
#include "effects/Effect.h"

class PulseCirclesEffect : public Effect
{
public:
    explicit PulseCirclesEffect(const String &id);
    void tick() override;
    void activate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

