#pragma once
#include "effects/fractional/FractionalEffect.h"

class FractorialFireEffect : public FractionalEffect
{
public:
    explicit FractorialFireEffect(const String &id);
    void tick() override;
};

