#pragma once
#include "effects/Effect.h"
class ClockHorizontal3Effect : public Effect
{
public:
    ClockHorizontal3Effect();
    void tick() override;
    void activate() override;
    void deactivate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

