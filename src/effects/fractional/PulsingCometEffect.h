#pragma once
#include "effects/fractional/FractionalEffect.h"

class PulsingCometEffect : public FractionalEffect
{
public:
    PulsingCometEffect();
    void tick() override;
};

