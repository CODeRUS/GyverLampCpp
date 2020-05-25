#pragma once
#include "effects/Effect.h"

class StarfallEffect : public Effect
{
public:
    explicit StarfallEffect(const String &id);
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

