#pragma once
#include <effects/Effect.h>

class ClockHorizontal1Effect : public Effect
{
public:
    explicit ClockHorizontal1Effect(const String &id);
    void tick() override;
    void activate() override;
    void deactivate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

