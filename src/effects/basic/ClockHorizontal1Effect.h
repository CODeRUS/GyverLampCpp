#pragma once
#include <effects/Effect.h>

class ClockHorizontal1Effect : public Effect
{
public:
    ClockHorizontal1Effect();
    void tick() override;
    void activate() override;
    void deactivate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

