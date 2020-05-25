#pragma once
#include "effects/fractional/FractionalEffect.h"

class RainbowKiteEffect : public FractionalEffect
{
public:
    explicit RainbowKiteEffect(const String &id);
    void tick() override;
};

