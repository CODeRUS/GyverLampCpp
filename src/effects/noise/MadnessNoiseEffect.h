#pragma once
#include "effects/noise/NoiseEffect.h"

class MadnessNoiseEffect : public NoiseEffect
{
public:
    MadnessNoiseEffect();
    void tick() override;
};

