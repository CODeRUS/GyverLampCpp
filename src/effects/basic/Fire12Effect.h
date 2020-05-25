#pragma once
#include "effects/Effect.h"

class Fire12Effect : public Effect
{
public:
    explicit Fire12Effect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

