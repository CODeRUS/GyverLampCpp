#pragma once
#include "effects/Effect.h"

class LightersEffect : public Effect
{
public:
    LightersEffect();
    void activate() override;
    void deactivate() override;
    void tick() override;

};

