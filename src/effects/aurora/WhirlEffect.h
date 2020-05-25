#pragma once
#include "AuroraEffect.h"

class WhirlEffect : public AuroraEffect
{
public:
    explicit WhirlEffect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

