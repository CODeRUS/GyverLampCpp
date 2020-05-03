#pragma once
#include "effects/Effect.h"

class RainEffect : public Effect
{
public:
    RainEffect();
    void tick() override;
};

