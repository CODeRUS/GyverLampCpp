#pragma once
#include "effects/Effect.h"

class MovingCubeEffect : public Effect
{
public:
    explicit MovingCubeEffect(const String &id);
    void tick() override;
    void activate() override;
    void deactivate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

