#pragma once
#include "effects/Effect.h"

class StormEffect : public Effect
{
public:
    explicit StormEffect(const String &id);
    void tick() override;
    void writeSettings(JsonObject &json) override;
    void initialize(const JsonObject &json) override;
};

