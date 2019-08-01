#pragma once
#include "effects/Effect.h"

class ClockEffect : public Effect
{
public:
    ClockEffect();
    void tick() override;
    void activate() override;
    void deactivate() override;
};

