#pragma once
#include "effects/Effect.h"

class SoundStereoEffect : public Effect
{
public:
    SoundStereoEffect();
    virtual void tick() override;

private:
    void displayLBand(int band);
    void displayRBand(int band);
    void renderSpectrometer();
    void captureSoundSample();
    byte getLBand(int i);
    byte getRBand(int i);
    void setBandwidth();
};
