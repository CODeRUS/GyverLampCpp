#pragma once
#include "effects/Effect.h"

class AnimationEffect : public Effect
{
public:
    AnimationEffect();
    void tick() override;
    void activate() override;
};

