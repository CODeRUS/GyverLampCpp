#pragma once
#include "effects/fractional/FractionalEffect.h"

class RainbowCometEffect : public FractionalEffect
{
public:
    explicit RainbowCometEffect(const String &id);
    void tick() override;
};

