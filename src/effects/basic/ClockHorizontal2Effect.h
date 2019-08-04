#pragma once
#include "effects/Effect.h"

class ClockHorizontal2Effect : public Effect
{
public:
    ClockHorizontal2Effect();
    void tick() override;
    void activate() override;
    void deactivate() override;
};

