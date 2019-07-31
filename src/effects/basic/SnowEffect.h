#pragma once
#include "effects/Effect.h"

class SnowEffect : public Effect
{
public:
    SnowEffect();
    void tick() override;
};

