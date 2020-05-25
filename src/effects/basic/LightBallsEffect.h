#pragma once
#include "effects/Effect.h"

class LightBallsEffect : public Effect
{
public:
    explicit LightBallsEffect(const String &id);
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

