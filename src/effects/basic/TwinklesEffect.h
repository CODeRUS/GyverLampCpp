#pragma once
#include "effects/Effect.h"

class TwinklesEffect : public Effect
{
public:
    explicit TwinklesEffect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

