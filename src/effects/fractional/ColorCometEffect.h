#pragma once
#include "effects/fractional/FractionalEffect.h"

class ColorCometEffect : public FractionalEffect
{
public:
    explicit ColorCometEffect(const String &id);
    void tick() override;
};

