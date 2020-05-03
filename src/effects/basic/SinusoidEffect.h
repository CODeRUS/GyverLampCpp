#pragma once
#include "effects/Effect.h"

class SinusoidEffect : public Effect
{
public:
    SinusoidEffect();
    void tick() override;
};

