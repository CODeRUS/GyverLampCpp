#pragma once
#include "effects/fractional/FractionalEffect.h"

class FractorialFireEffect : public FractionalEffect
{
public:
    FractorialFireEffect();
    void tick() override;
};

