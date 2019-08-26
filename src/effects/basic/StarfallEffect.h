#pragma once
#include "effects/Effect.h"

class StarfallEffect : public Effect
{
public:
    StarfallEffect();
    void tick() override;
};

