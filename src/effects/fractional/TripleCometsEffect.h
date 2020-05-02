#pragma once
#include "effects/fractional/FractionalEffect.h"

class TripleCometsEffect : public FractionalEffect
{
public:
    TripleCometsEffect();
    void tick() override;
};

