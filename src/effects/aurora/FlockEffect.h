#pragma once
#include "AuroraEffect.h"

class FlockEffect : public AuroraEffect
{
public:
    explicit FlockEffect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

