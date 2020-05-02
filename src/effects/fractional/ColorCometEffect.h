#pragma once
#include "effects/fractional/FractionalEffect.h"

class ColorCometEffect : public FractionalEffect
{
public:
    ColorCometEffect();
    void tick() override;
};

