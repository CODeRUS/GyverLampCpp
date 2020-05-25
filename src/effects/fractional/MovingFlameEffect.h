#pragma once
#include "effects/fractional/FractionalEffect.h"

class MovingFlameEffect : public FractionalEffect
{
public:
    explicit MovingFlameEffect(const String &id);
    void tick() override;
};

