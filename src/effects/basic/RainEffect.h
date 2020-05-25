#pragma once
#include "effects/Effect.h"

class RainEffect : public Effect
{
public:
    explicit RainEffect(const String &id);
    void tick() override;
};

