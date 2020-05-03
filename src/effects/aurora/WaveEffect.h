#pragma once
#include "effects/Effect.h"

class WaveEffect : public Effect
{
public:
    WaveEffect();
    void activate() override;
    void tick() override;
};

