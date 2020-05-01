#pragma once
#include "effects/Effect.h"

class WaterfallEffect : public Effect
{
public:
    WaterfallEffect();
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;

};
