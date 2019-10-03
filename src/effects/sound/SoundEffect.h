#pragma once
#include "effects/Effect.h"

class SoundEffect : public Effect
{
public:
    SoundEffect();
    virtual void tick() override;

private:
    void displayBand(int band, int dsize);
    byte getBand(int i);
    void setBandwidth();
};
