#pragma once
#include "effects/fractional/FractionalEffect.h"

class DoubleCometsEffect : public FractionalEffect
{
public:
    DoubleCometsEffect();
    void tick() override;
};

