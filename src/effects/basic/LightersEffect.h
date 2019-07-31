#pragma once
#include "effects/Effect.h"

class LightersEffect : public Effect
{
public:
    LightersEffect();
    void tick() override;
};

