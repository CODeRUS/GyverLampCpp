#pragma once
#include "effects/fractional/FractionalEffect.h"

class PulsingCometEffect : public FractionalEffect
{
public:
    explicit PulsingCometEffect(const String &id);
    void tick() override;
};

