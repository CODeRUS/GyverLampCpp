#pragma once
#include "effects/Effect.h"

class BouncingBallsEffect : public Effect
{
public:
    explicit BouncingBallsEffect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

