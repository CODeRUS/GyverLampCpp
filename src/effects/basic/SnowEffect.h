#pragma once
#include "effects/Effect.h"

class SnowEffect : public Effect
{
public:
    explicit SnowEffect(const String &id);
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

