#pragma once
#include "AuroraEffect.h"

class FlockEffect : public AuroraEffect
{
public:
    FlockEffect();
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

