#pragma once
#include "effects/fractional/FractionalEffect.h"

class RainbowKiteEffect : public FractionalEffect
{
public:
    RainbowKiteEffect();
    void tick() override;
};

