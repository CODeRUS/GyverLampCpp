#pragma once
#include "effects/fractional/FractionalEffect.h"

class DoubleCometsEffect : public FractionalEffect
{
public:
    explicit DoubleCometsEffect(const String &id);
    void tick() override;
};

