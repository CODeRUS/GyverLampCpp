#pragma once
#include "effects/Effect.h"

class ColorEffect : public Effect
{
public:
    ColorEffect();
    void tick() override;
};

