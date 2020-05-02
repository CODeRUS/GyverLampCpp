#pragma once
#include "effects/Effect.h"

class LightBallsEffect : public Effect
{
public:
    LightBallsEffect();
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

