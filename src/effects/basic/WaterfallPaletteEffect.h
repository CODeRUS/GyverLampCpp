#pragma once
#include "effects/Effect.h"

class WaterfallPaletteEffect : public Effect
{
public:
    WaterfallPaletteEffect();
    void tick() override;
    void activate() override;
    void deactivate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

