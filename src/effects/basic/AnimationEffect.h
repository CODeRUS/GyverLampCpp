#pragma once
#include "effects/Effect.h"

class AnimationEffect : public Effect
{
public:
    explicit AnimationEffect(const String &id);
    void tick() override;
    void activate() override;
};

