#pragma once
#include "effects/fractional/FractionalEffect.h"

class MovingFlameEffect : public FractionalEffect
{
public:
    MovingFlameEffect();
    void tick() override;
};

