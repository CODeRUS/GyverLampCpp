#pragma once
#include "effects/noise/NoiseEffect.h"

class MadnessNoiseEffect : public NoiseEffect
{
public:
    explicit MadnessNoiseEffect(const String &id);
    void tick() override;
};

