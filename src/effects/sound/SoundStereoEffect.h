#pragma once
#include "effects/Effect.h"

class SoundStereoEffect : public Effect
{
public:
    explicit SoundStereoEffect(const String &id);
    virtual void tick() override;

private:
    void displayLBand(int band);
    void displayRBand(int band);
    byte getLBand(int i);
    byte getRBand(int i);
    void setBandwidth();
};
