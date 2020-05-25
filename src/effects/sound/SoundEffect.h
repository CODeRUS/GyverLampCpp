#pragma once
#include "effects/Effect.h"

class SoundEffect : public Effect
{
public:
    explicit SoundEffect(const String &id);
    virtual void tick() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;

private:
    void displayBand(int band, int dsize);
    byte getBand(int i);
    void setBandwidth();
};
