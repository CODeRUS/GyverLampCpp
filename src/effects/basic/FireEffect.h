#pragma once
#include "effects/Effect.h"

class FireEffect : public Effect
{
public:
    explicit FireEffect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;

private:
    void generateLine();
    void shiftUp();
    void drawFrame(uint8_t pcnt);
};

