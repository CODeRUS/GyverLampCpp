#pragma once
#include "effects/Effect.h"

class ClockEffect : public Effect
{
public:
    explicit ClockEffect(const String &id);
    void tick() override;
    void activate() override;
    void deactivate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

