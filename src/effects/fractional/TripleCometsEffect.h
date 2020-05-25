#pragma once
#include "effects/fractional/FractionalEffect.h"

class TripleCometsEffect : public FractionalEffect
{
public:
    explicit TripleCometsEffect(const String &id);
    void tick() override;
};

