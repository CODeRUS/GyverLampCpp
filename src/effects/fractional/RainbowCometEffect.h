#pragma once
#include "effects/fractional/FractionalEffect.h"

class RainbowCometEffect : public FractionalEffect
{
public:
    RainbowCometEffect();
    void tick() override;
};

