#pragma once
#include "effects/Effect.h"

class WhiteColorEffect : public Effect
{
public:
    WhiteColorEffect();
    void tick() override;
};

